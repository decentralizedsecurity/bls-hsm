function setCommand(commandId){
  let command
  switch(commandId){
    case 'upcheck':
      command = 'GET /upcheck HTTP/1.1\nHost: localhost:8080\n\n'
      break;
    case 'publicKey':
      command = 'GET /api/v1/eth2/publicKeys HTTP/1.1\nHost: localhost:8080\n\n'
      break;
    case 'importKeystore':
      command = 'POST /eth/v1/keystores HTTP/1.1\nHost: localhost:8080\nContent-Type: application/json\nContent-Length: 1212\n\n{\n    "keystores": [\n        "{\n    \\"crypto\\": {\n        \\"kdf\\": {\n            \\"function\\": \\"pbkdf2\\",\n            \\"message\\": \\"\\",\n            \\"params\\": {\n                \\"dklen\\": 32,\n                \\"c\\": 262144,\n                \\"prf\\": \\"hmac-sha256\\",\n                \\"salt\\": \\"8123ea083eae312143c724a8063ea9ec53b4818d34726b28a20fafa6107b2900\\"\n            }\n        },\n        \\"checksum\\": {\n            \\"function\\": \\"sha256\\",\n            \\"params\\": {},\n            \\"message\\": \\"e5b9369f4f60f6c8bf4982254a7c1989e243d1ce372af4a6b0ad50a20f33bfa4\\"\n        },\n        \\"cipher\\": {\n            \\"function\\": \\"aes-128-ctr\\",\n            \\"params\\": {\n                \\"iv\\": \\"7ea5abd19a7747ddac97b3951ade63a5\\"\n            },\n            \\"message\\": \\"ac099b2371f36ed9356e03918ddbcb231b3d018ef5d3b5b5041ecc2b83d56a2a\\"\n        }\n    },\n    \\"description\\": \\"\\",\n    \\"pubkey\\": \\"ae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2\\",\n    \\"path\\": \\"m/12381/3600/0/0/0\\",\n    \\"uuid\\": \\"cc260592-1cf5-40d7-bc5a-44eaaa298d06\\",\n    \\"version\\": 4\n}"\n    ],\n    "passwords": [\n        "123456789"\n    ]\n}';
      break;
    case 'importKeystoreWithoutPwd':
      command = 'POST /eth/v1/keystores HTTP/1.1\nHost: localhost:8080\nContent-Type: application/json\nContent-Length: 1212\n\n{\n    "keystores": [\n        "{\n    \\"crypto\\": {\n        \\"kdf\\": {\n            \\"function\\": \\"pbkdf2\\",\n            \\"message\\": \\"\\",\n            \\"params\\": {\n                \\"dklen\\": 32,\n                \\"c\\": 262144,\n                \\"prf\\": \\"hmac-sha256\\",\n                \\"salt\\": \\"8123ea083eae312143c724a8063ea9ec53b4818d34726b28a20fafa6107b2900\\"\n            }\n        },\n        \\"checksum\\": {\n            \\"function\\": \\"sha256\\",\n            \\"params\\": {},\n            \\"message\\": \\"e5b9369f4f60f6c8bf4982254a7c1989e243d1ce372af4a6b0ad50a20f33bfa4\\"\n        },\n        \\"cipher\\": {\n            \\"function\\": \\"aes-128-ctr\\",\n            \\"params\\": {\n                \\"iv\\": \\"7ea5abd19a7747ddac97b3951ade63a5\\"\n            },\n            \\"message\\": \\"ac099b2371f36ed9356e03918ddbcb231b3d018ef5d3b5b5041ecc2b83d56a2a\\"\n        }\n    },\n    \\"description\\": \\"\\",\n    \\"pubkey\\": \\"ae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2\\",\n    \\"path\\": \\"m/12381/3600/0/0/0\\",\n    \\"uuid\\": \\"cc260592-1cf5-40d7-bc5a-44eaaa298d06\\",\n    \\"version\\": 4\n}"\n    ],\n    "passwords": []\n}';
      break;
    case 'fastImportKeystore':
      command = 'POST /eth/v1/keystores HTTP/1.1\nHost: localhost:8080\nContent-Type: application/json\nContent-Length: 1207\n\n{\n    "keystores": [\n        "{\n    \\"crypto\\": {\n        \\"kdf\\": {\n            \\"function\\": \\"pbkdf2\\",\n            \\"message\\": \\"\\",\n            \\"params\\": {\n                \\"dklen\\": 32,\n                \\"c\\": 8,\n                \\"prf\\": \\"hmac-sha256\\",\n                \\"salt\\": \\"8123ea083eae312143c724a8063ea9ec53b4818d34726b28a20fafa6107b2900\\"\n            }\n        },\n        \\"checksum\\": {\n            \\"function\\": \\"sha256\\",\n            \\"params\\": {},\n            \\"message\\": \\"29658a4b9ba50513437ee4bc1e907d3d70106da7dad41ae0b4f8a481aab8dbb2\\"\n        },\n        \\"cipher\\": {\n            \\"function\\": \\"aes-128-ctr\\",\n            \\"params\\": {\n                \\"iv\\": \\"7ea5abd19a7747ddac97b3951ade63a5\\"\n            },\n            \\"message\\": \\"3702e8df43331dfd1014162c1ee0152bbbab2536368249e4d3b2bcb26eabfa94\\"\n        }\n    },\n    \\"description\\": \\"\\",\n    \\"pubkey\\": \\"ae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2\\",\n    \\"path\\": \\"m/12381/3600/0/0/0\\",\n    \\"uuid\\": \\"cc260592-1cf5-40d7-bc5a-44eaaa298d06\\",\n    \\"version\\": 4\n}"\n    ],\n    "passwords": [\n        "123456789"\n    ]\n}';
      break;
    case 'fastImportKeystoreWithoutPwd':
      command = 'POST /eth/v1/keystores HTTP/1.1\nHost: localhost:8080\nContent-Type: application/json\nContent-Length: 1198\n\n{\n    "keystores": [\n        "{\n    \\"crypto\\": {\n        \\"kdf\\": {\n            \\"function\\": \\"pbkdf2\\",\n            \\"message\\": \\"\\",\n            \\"params\\": {\n                \\"dklen\\": 32,\n                \\"c\\": 8,\n                \\"prf\\": \\"hmac-sha256\\",\n                \\"salt\\": \\"8123ea083eae312143c724a8063ea9ec53b4818d34726b28a20fafa6107b2900\\"\n            }\n        },\n        \\"checksum\\": {\n            \\"function\\": \\"sha256\\",\n            \\"params\\": {},\n            \\"message\\": \\"29658a4b9ba50513437ee4bc1e907d3d70106da7dad41ae0b4f8a481aab8dbb2\\"\n        },\n        \\"cipher\\": {\n            \\"function\\": \\"aes-128-ctr\\",\n            \\"params\\": {\n                \\"iv\\": \\"7ea5abd19a7747ddac97b3951ade63a5\\"\n            },\n            \\"message\\": \\"3702e8df43331dfd1014162c1ee0152bbbab2536368249e4d3b2bcb26eabfa94\\"\n        }\n    },\n    \\"description\\": \\"\\",\n    \\"pubkey\\": \\"ae249bcf645e7470cdd10c546de97ea87f70a93dbf8a99e2b77833c9e83a5833a6d37f73ef8359aa79f495130697eec2\\",\n    \\"path\\": \\"m/12381/3600/0/0/0\\",\n    \\"uuid\\": \\"cc260592-1cf5-40d7-bc5a-44eaaa298d06\\",\n    \\"version\\": 4\n}"\n    ],\n    "passwords": [\n        ""\n    ]\n}';
      break;
  }
  document.getElementById('command').value = command
}

function sendCommand() {
  var commandText = document.getElementById("command").value;
  
  console.log(commandText); // Textarea content
  // From \n to \r\n
  var modifiedCommandText = commandText.replace(/\n/g, '\r\n'); 
  //console.log(modifiedCommandText); // Modified content

  sendCommandSerial(modifiedCommandText, 4);
}

// Sends a message through serial port and receives a response
function sendCommandSerial(message, nResponses) {
  let port;
  let reader;

  // Request access to serial port (no filters)
  navigator.serial.requestPort().then((selectedPort) => {
    port = selectedPort;
    return port.open({ baudRate: 115200 });
  }).then(() => {
    // Convert message to byte string
    const encoder = new TextEncoder();
    const data = encoder.encode(message);
    // Get serial port writer
    const writer = port.writable.getWriter();
    // Send message to serial port
    return writer.write(data);
  }).then(() => {
    // Get serial port reader
    reader = port.readable.getReader();
    readResponses(nResponses);
  }).catch((error) => {
    console.error('Error:', error);
  });
  
    function readResponses(remainingResponses) {
      if (remainingResponses <= 0) {
        // Close reader when every responses have been displayed
        reader.releaseLock();
        return;
      }
  
      // Read response
      reader.read().then((result) => {
        // Decode received data
        const decoder = new TextDecoder();
        const response = decoder.decode(result.value);
        console.log('Response:', response);
        const responseText = response.replace(/\r?\n|\r/g, '<br>')
        document.getElementById('infoContainer').innerHTML = document.getElementById('infoContainer').innerHTML + responseText

        readResponses(remainingResponses - 1);
      });
    }
  }