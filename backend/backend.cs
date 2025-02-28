using System.Linq;
using System;
using System.IO.Ports;
using System.Net;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;
using Newtonsoft.Json;

class Backend {

    static SerialPort serialPort = new SerialPort("COM3", 9600); // Serial Port Connection
    static HttpListener listener = new HttpListener(); // HTTP Web socket
    static List<WebSocket> webSocketClients = new List<WebSocket>(); // Store WebSocket clients

    static async Task Main() {
        // Handle app exit to close the serial port
        AppDomain.CurrentDomain.ProcessExit += (s, e) => {
            if (serialPort.IsOpen) {
                //Console.WriteLine("Closing Serial Port...");
                serialPort.Close();
            }
        };

        // Serial port to receive the incoming serial data
        serialPort.DataReceived += SerialDataReceived;

        // Opens the serial communications
        serialPort.Open();
        //Console.WriteLine("Serial Port open and waiting for data...");

        listener.Prefixes.Add("http://localhost:8080/");
        listener.Start();
        //Console.WriteLine("WebSocket Server Started on LocalHost...");

        // Send data to each connected WebSocket client
        while (listener.IsListening) {
            HttpListenerContext context = await listener.GetContextAsync(); // Waits for a client request

            // Handle WebSocket connection
            if (context.Request.IsWebSocketRequest) {
                WebSocket webSocket = (await context.AcceptWebSocketAsync(null)).WebSocket;
                //Console.WriteLine("WebSocket connection established...");
                webSocketClients.Add(webSocket); // Add WebSocket client to list

                // Handle incoming serial data and broadcast it via WebSocket
                _ = HandleWebSocketConnectionAsync(webSocket); // Handle WebSocket connection asynchronously
            }
            else {
                context.Response.StatusCode = 400;  // Bad request if not WebSocket
                context.Response.Close();
            }
        }
    }

    static string temperature = null;
    static string humidity = null;
    static string lightLevel = null;

    static async void SerialDataReceived(object sender, SerialDataReceivedEventArgs e) {
        try {
            if (!serialPort.IsOpen) {
                serialPort.Open();
            }

            string data = serialPort.ReadLine().Trim();  // Read line and trim extra spaces or newlines

            // Parse the data (it will be like t 68.36 F, h 39.00 %, l 13 lux)
            var dataDict = ParseSerialData(data);

            // Check and store the data accordingly
            if (dataDict.ContainsKey("t")) {
                temperature = dataDict["t"];
            } else if (dataDict.ContainsKey("h")) {
                humidity = dataDict["h"];
            } else if (dataDict.ContainsKey("l")) {
                lightLevel = dataDict["l"];
            }

            // If all required data is present, send it together
            if (temperature != null && humidity != null && lightLevel != null) {
                var fullData = new Dictionary<string, string>
                {
                    { "t", temperature },
                    { "h", humidity },
                    { "l", lightLevel }
                };

                // Broadcast the complete data to WebSocket clients
                await BroadcastToWebSocketClients(fullData);

                // Reset values if you need to prepare for the next set of data
                temperature = null;
                humidity = null;
                lightLevel = null;
            }

        } catch (Exception ex) {
            Console.WriteLine($"Error: {ex.Message}");
        }
    }


    static Dictionary<string, string> ParseSerialData(string data) {
        Dictionary<string, string> dataDict = new Dictionary<string, string>();
            Console.WriteLine(data);

        var parts = data.Split(new[] { " " }, StringSplitOptions.RemoveEmptyEntries);

        if (parts.Length >= 2) {
            string key = parts[0];
            // Handle other keys and values
            string v = string.Join(" ", parts.Skip(1)); // Join all parts after the first one as the value
            dataDict[key] = v;
            // Console.WriteLine(key + " " + v);
        }

        return dataDict;
    }

    static async Task HandleWebSocketConnectionAsync(WebSocket webSocket) {
        webSocketClients.Add(webSocket); // Add WebSocket to the global list of clients
        byte[] buffer = new byte[1024];

        try {
            while (webSocket.State == WebSocketState.Open) {
                // Wait for incoming messages (not required here, just for receiving messages if needed)
                await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
            }
        } catch (Exception ex) {
            //Console.WriteLine($"WebSocket error: {ex.Message}");
        } finally {
            webSocketClients.Remove(webSocket); // Remove the client when it's disconnected
            webSocket?.Dispose();
        }
    }


    static async Task BroadcastToWebSocketClients(Dictionary<string, string> data) {
        // Serialize the entire dictionary as a JSON object
        string jsonData = JsonConvert.SerializeObject(data); 

        Console.WriteLine($"Sending data to clients: {jsonData}");

        // Send to all connected WebSocket clients
        foreach (var client in webSocketClients) {
            try {
                if (client.State == WebSocketState.Open) {
                    var buffer = Encoding.UTF8.GetBytes(jsonData);
                    await client.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Text, true, CancellationToken.None);
                    Console.WriteLine("Sent data to WebSocket client");
                }
            } catch (Exception ex) {
                Console.WriteLine($"Error broadcasting data: {ex.Message}");
            }
        }
    }

}
