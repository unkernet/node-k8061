{
  "targets": [
    {
      "target_name": "k8061",
      "sources": [ "k8061.cc" ],
      "libraries": [ "-lusb" ]
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "k8061" ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/k8061.node" ],
          "destination": "."
        }
      ]
    }
  ]
}
