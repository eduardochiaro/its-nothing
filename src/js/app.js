var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

// Weather functionality
var lastWeatherData = null; // Store last weather data for unit conversion

function getLocation(successCallback, errorCallback) {
  navigator.geolocation.getCurrentPosition(
    function(position) {
      successCallback({
        lat: position.coords.latitude,
        lon: position.coords.longitude
      });
    },
    function(error) {
      errorCallback(error);
    },
    { timeout: 15000, maximumAge: 300000 } // 5 minute cache
  );
}

function celsiusToFahrenheit(celsius) {
  return Math.round((celsius * 9/5) + 32);
}

function fetchWeather(lat, lon, useF, successCallback, errorCallback) {
  if (useF === undefined) useF = false;
  // Always fetch in Celsius and convert client-side if needed
  var url = 'https://api.open-meteo.com/v1/forecast?latitude=' + lat + '&longitude=' + lon + '&current_weather=true&temperature_unit=celsius';
  
  var xhr = new XMLHttpRequest();
  
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4) {
      if (xhr.status === 200) {
        try {
          var data = JSON.parse(xhr.responseText);
          if (data.current_weather) {
            var tempCelsius = data.current_weather.temperature;
            var temperature = useF ? celsiusToFahrenheit(tempCelsius) : Math.round(tempCelsius);
            var condition = getWeatherCondition(data.current_weather.weathercode);
            
            // Store weather data for unit conversion
            lastWeatherData = {
              tempCelsius: tempCelsius,
              condition: condition
            };
            
            successCallback({
              temperature: temperature,
              condition: condition
            });
          } else {
            errorCallback('Invalid weather data received');
          }
        } catch (parseError) {
          errorCallback('JSON parse error: ' + parseError.message);
        }
      } else {
        errorCallback('HTTP error! status: ' + xhr.status);
      }
    }
  };
  
  xhr.onerror = function() {
    errorCallback('Network error occurred');
  };
  
  xhr.ontimeout = function() {
    errorCallback('Request timed out');
  };
  
  xhr.timeout = 10000; // 10 second timeout
  xhr.open('GET', url, true);
  xhr.send();
}

function getWeatherCondition(weatherCode) {
  // Open-Meteo weather codes to simple conditions
  if (weatherCode === 0) return 'CLEAR';
  if (weatherCode >= 1 && weatherCode <= 3) return 'CLOUDY';
  if (weatherCode >= 45 && weatherCode <= 48) return 'FOG';
  if (weatherCode >= 51 && weatherCode <= 67) return 'RAIN';
  if (weatherCode >= 71 && weatherCode <= 77) return 'SNOW';
  if (weatherCode >= 80 && weatherCode <= 82) return 'SHOWER';
  if (weatherCode >= 95 && weatherCode <= 99) return 'STORM';
  return 'UNKNOWN';
}

function updateWeather(useFahrenheit = false) {
  console.log('Fetching weather data... (' + (useFahrenheit ? 'Fahrenheit' : 'Celsius') + ')');
  
  getLocation(
    function(location) {
      console.log('Got location: ' + location.lat + ', ' + location.lon);
      
      fetchWeather(location.lat, location.lon, useFahrenheit,
        function(weather) {
          var unit = useFahrenheit ? 'F' : 'C';
          console.log('Weather: ' + weather.temperature + '°' + unit + ', ' + weather.condition);
          
          // Send weather data to watch
          Pebble.sendAppMessage({
            'WeatherTemperature': weather.temperature,
            'WeatherCondition': weather.condition
          }, 
          function() {
            console.log('Weather data sent successfully');
          },
          function(error) {
            console.log('Error sending weather: ' + error);
          });
        },
        function(error) {
          console.log('Weather fetch error: ' + error);
          // Send fallback data
          Pebble.sendAppMessage({
            'WeatherTemperature': 0,
            'WeatherCondition': 'ERROR'
          });
        }
      );
    },
    function(error) {
      console.log('Location error: ' + error);
      // Send fallback data
      Pebble.sendAppMessage({
        'WeatherTemperature': 0,
        'WeatherCondition': 'NO_GPS'
      });
    }
  );
}

// Update weather on app start and every 30 minutes
Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
  
  // Initialize temperature unit from stored settings
  var settings = JSON.parse(localStorage.getItem('clay-settings') || '{}');
  if (settings.WeatherUseFahrenheit !== undefined) {
    var useFahrenheit = settings.WeatherUseFahrenheit;
    console.log('Initialized temperature unit: ' + (useFahrenheit ? 'Fahrenheit' : 'Celsius'));
  }
  
  updateWeather(useFahrenheit);
  
  // Update weather every 30 minutes
  setInterval(updateWeather, 30 * 60 * 1000);
});

// Listen for Clay configuration changes
Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }
  
  var claySettings = clay.getSettings(e.response);
  
  var newUseFahrenheit = claySettings.WeatherUseFahrenheit;
  console.log('Fahrenheit setting from config: ' + newUseFahrenheit);

  // If we have cached weather data, convert it instead of refetching
  if (lastWeatherData) {
    var temperature = claySettings.WeatherUseFahrenheit ? 
      celsiusToFahrenheit(lastWeatherData.tempCelsius) : 
      Math.round(lastWeatherData.tempCelsius);
      
    console.log('Converting cached temperature: ' + temperature + '°' + (newUseFahrenheit ? 'F' : 'C'));
    
    Pebble.sendAppMessage({
      'WeatherTemperature': temperature,
      'WeatherCondition': lastWeatherData.condition
    });
  } else {
    // No cached data, fetch new weather
    setTimeout(function() {
      updateWeather(claySettings.WeatherUseFahrenheit);
    }, 1000);
  }
  
  // Send all configuration to watch
  Pebble.sendAppMessage(claySettings,
    function(e) {
      console.log('Configuration sent successfully');
    },
    function(e) {
      console.log('Failed to send configuration: ' + JSON.stringify(e));
    }
  );
});