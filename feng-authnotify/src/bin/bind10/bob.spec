{
  "module_spec": {
    "module_name": "Boss",
    "module_description": "Master process",
    "config_data": [
      {
        "item_name": "example_string",
        "item_type": "string",
        "item_optional": False,
        "item_default": "Just an example string configuration value"
      },
      {
        "item_name": "example_int",
        "item_type": "integer",
        "item_optional": False,
        "item_default": 1
      }
    ],
    "commands": [
      {
        "command_name": "print_message",
        "command_description": "Print the given message to stdout",
        "command_args": [ {
          "item_name": "message",
          "item_type": "string",
          "item_optional": False,
          "item_default": ""
        } ]
      },
      {
        "command_name": "print_settings",
        "command_description": "Print some_string and some_int to stdout",
        "command_args": []
      },
      {
        "command_name": "shutdown",
        "command_description": "Shut down BIND 10",
        "command_args": []
      }
    ]
  }
}


