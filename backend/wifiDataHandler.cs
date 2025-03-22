using System;
using System.Net.Http;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Collections.Generic;
using System.Linq;

class WifiDataHandler
{
    // Arduino Nano 33 IoT IP address (change this to your device's IP address)
     static string arduinoIp = "http://192.168.4.152";  // Replace with your Arduino's IP address
    static int port = 6969;

    private static readonly HttpClient client = new HttpClient(); // Reusable HttpClient

    private string temperature = string.Empty;
    private string humidity = string.Empty;
    private string lightLevel = string.Empty;
    private string distance = string.Empty;
    private string motion = string.Empty;

    public async Task DataCollector() {
        string url = $"{arduinoIp}:{port}";

        Console.WriteLine($"Attempting to connect to {url}...");

        try {
            HttpResponseMessage response = await client.GetAsync(url);
            Console.WriteLine($"Response status: {response.StatusCode}");

            if (response.IsSuccessStatusCode) {
                string data = await response.Content.ReadAsStringAsync();
                Console.WriteLine($"Received data: {data}");
                await handleData(data);
            }
            else {
                Console.WriteLine($"Failed to get data from Arduino. Status code: {response.StatusCode}");
            }
        }
        catch (Exception ex) {
            Console.WriteLine($"Error occurred while connecting to Arduino: {ex}");
        }
    }


    public async Task handleData(string data) {
        try {
            var dataDict = ParseReceivedData(data);

            // Assign values from the dictionary to the fields
            if (dataDict.ContainsKey("t")) {
                temperature = dataDict["t"];
            }
            if (dataDict.ContainsKey("h")) {
                humidity = dataDict["h"];
            }
            if (dataDict.ContainsKey("l")) {
                lightLevel = dataDict["l"];
            }
            if (dataDict.ContainsKey("d")) {
                distance = dataDict["d"];
            }
            if (dataDict.ContainsKey("m")) {
                motion = dataDict["m"];
            }

            // Send data to the WebSocket if all fields are filled
            if (!string.IsNullOrEmpty(temperature) && !string.IsNullOrEmpty(humidity) &&
                !string.IsNullOrEmpty(lightLevel) && !string.IsNullOrEmpty(distance) &&
                !string.IsNullOrEmpty(motion)) {
                var fullData = new Dictionary<string, string> {
                    { "t", temperature },
                    { "h", humidity },
                    { "l", lightLevel },
                    { "d", distance },
                    { "m", motion }
                };

                // Call WebSocketHandler to broadcast data
                await WebSocketHandler.BroadcastToWebSocketClients(fullData);

                // Reset data for next collection
                temperature = string.Empty;
                humidity = string.Empty;
                lightLevel = string.Empty;
                distance = string.Empty;
                motion = string.Empty;
            }
        }
        catch (Exception ex) {
            Console.WriteLine($"Wifi Data Error: {ex.Message}");
        }
    }


    private Dictionary<string, string> ParseReceivedData(string data) {
        Dictionary<string, string> dataDict = new Dictionary<string, string>();

        // Split the incoming data string by newline
        var parts = data.Split(new[] { "\n" }, StringSplitOptions.RemoveEmptyEntries);
        
        foreach (var p in parts) {
            int index = p.IndexOf(" ");
            string key = p.Substring(0, index);
            string value = p.Substring(index + 1);
            dataDict[key] = value;
        }

        return dataDict;
    }
}
