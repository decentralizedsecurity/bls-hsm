function setCommand(commandId){
  let command
  switch(commandId){
    case 'upcheck':
      command = 'GET /upcheck HTTP/1.1\nHost: localhost:8080\n\n'
      break;
    case 'publicKey':
      command = 'GET /api/v1/eth2/publicKeys HTTP/1.1\nHost: localhost:8080\n\n'
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

        readResponses(remainingResponses - 1);
      });
    }
  }