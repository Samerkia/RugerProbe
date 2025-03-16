using System;
using System.Net;
using System.Net.WebSockets;
using System.Threading.Tasks;
using InTheHand.Net.Sockets;
using InTheHand.Net.Bluetooth;

class Backend
{
    static HttpListener listener = new HttpListener(); // HTTP WebSocket Listener

    static async Task Main() {
        Backend backend = new Backend();
        await backend.UseSerial();
    }

    public async Task UseSerial() {
        // Set up Serial Data Handler
        var serialDataHandler = new SerialDataHandler("COM5", 115200);
        serialDataHandler.Start();

        // Set up WebSocket Listener
        listener.Prefixes.Add("http://localhost:8080/");
        listener.Start();

        Console.WriteLine("WebSocket Server Started on LocalHost...");

        // Listen for incoming HTTP WebSocket requests
        while (listener.IsListening)
        {
            HttpListenerContext context = await listener.GetContextAsync();

            if (context.Request.IsWebSocketRequest)
            {
                WebSocket webSocket = (await context.AcceptWebSocketAsync(null)).WebSocket;
                Console.WriteLine("WebSocket connection established...");
                _ = WebSocketHandler.HandleWebSocketConnectionAsync(webSocket);
            }
            else
            {
                context.Response.StatusCode = 400;  // Bad request if not WebSocket
                context.Response.Close();
            }
        }

        // Handle app exit to close serial port
        AppDomain.CurrentDomain.ProcessExit += (s, e) =>
        {
            serialDataHandler.Stop();
        };
    }
}
