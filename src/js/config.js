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
        "label": "Foreground Color"
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
        "defaultValue": true,
        "description": "Use 24-Hours or 12 Hours format."
      },
      {
        "type": "toggle",
        "messageKey": "SettingShowAMPM",
        "label": "Show AM/PM",
        "defaultValue": true,
        "description": "Display AM/PM next to minutes, only works if `Use 24-Hour Format` setting is off."
      },
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];