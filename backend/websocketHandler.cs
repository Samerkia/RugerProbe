using System;
using System.Net.WebSockets;
using System.Text;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Threading;

public static class WebSocketHandler
{
    private static List<WebSocket> webSocketClients = new List<WebSocket>();

    public static async Task HandleWebSocketConnectionAsync(WebSocket webSocket)
    {
        webSocketClients.Add(webSocket);
        byte[] buffer = new byte[1024];

        try
        {
            while (webSocket.State == WebSocketState.Open)
            {
                // Receive message from client (if needed for future expansion)
                await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"WebSocket error: {ex.Message}");
        }
        finally
        {
            webSocketClients.Remove(webSocket);
            webSocket?.Dispose();
        }
    }

    public static async Task BroadcastToWebSocketClients(Dictionary<string, string> data)
    {
        string jsonData = Newtonsoft.Json.JsonConvert.SerializeObject(data); // Serialize the dictionary to JSON
        Console.WriteLine($"Sending data to clients: {jsonData}");

        foreach (var client in webSocketClients)
        {
            try
            {
                if (client.State == WebSocketState.Open)
                {
                    var buffer = Encoding.UTF8.GetBytes(jsonData);
                    await client.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Text, true, CancellationToken.None);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error broadcasting data: {ex.Message}");
            }
        }
    }
}
