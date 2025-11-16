module.exports = [
  {
    "type": "heading",
    "defaultValue": "It's Nothing Settings"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColor",
        "defaultValue": "0x000000",
        "label": "Background Color"
      },
      {
        "type": "color",
        "messageKey": "ForegroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Text Color"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Time Settings"
      },
      {
        "type": "toggle",
        "messageKey": "Setting24H",
        "label": "Use 24-Hour Format",
        "defaultValue": false,
        "description": "Use 24-Hours or 12 Hours format."
      },
      {
        "type": "toggle",
        "messageKey": "SettingShowAMPM",
        "label": "Show AM/PM",
        "defaultValue": true,
        "description": "Display AM/PM next to minutes, only works if `Use 24-Hour Format` setting is off."
      },
      {
        "type": "toggle",
        "messageKey": "SettingShowSeconds",
        "label": "Show Seconds",
        "defaultValue": false,
        "description": "Display seconds above the time. Updates every second when enabled."
      },
      {
        "type": "toggle",
        "messageKey": "DotThickness",
        "label": "Make Number Dots Thicker",
        "defaultValue": false,
        "description": "Make the dots in the numbers thicker."
      },

    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Modules"
      },
      {
        "type": "select",
        "messageKey": "TopModule",
        "defaultValue": "date",
        "label": "Top Module",
        "options": [
          { 
            "label": "Empty", 
            "value": "" 
          },
          { 
            "label": "Date",
            "value": "date" 
          },
          { 
            "label": "Steps",
            "value": "steps" 
          },
          { 
            "label": "Weather",
            "value": "weather" 
          }
        ]
      },
      {
        "type": "select",
        "messageKey": "BottomModule",
        "defaultValue": "steps",
        "label": "Bottom Module",
        "options": [
          { 
            "label": "Empty", 
            "value": "" 
          },
          { 
            "label": "Date",
            "value": "date" 
          },
          { 
            "label": "Steps",
            "value": "steps" 
          },
          { 
            "label": "Weather",
            "value": "weather" 
          }
        ]
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Weather"
      },
      {
        "type": "toggle",
        "messageKey": "WeatherUseFahrenheit",
        "label": "Use Fahrenheit",
        "defaultValue": false,
        "description": "Display temperature in Fahrenheit instead of Celsius."
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];