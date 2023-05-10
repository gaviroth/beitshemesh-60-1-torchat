using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Numerics;
using System.Text;
using System.Windows;
using System.Data.SQLite;
using System.Threading;

namespace WpfApp1
{
    /// <summary>
    /// Interaction logic for Menu.xaml
    /// </summary>
    public partial class Menu : Window
    {
        static public string my_username;
        static public SQLiteConnection my_conn;
        static public int my_private_key1;
        static public int my_num1;
        static public int my_public_key1;
        static public int PORT1;
        static public int token1;
        static public string sign = "";
        static public bool should_exist = true;
        static public Thread thread;


        public Menu(string username, SQLiteConnection conn,int my_private_key,int my_num,int my_public_key,int PORT,int token)
        {
            my_conn = conn;
            my_username = username;
            my_private_key1 = my_private_key;
            my_num1 = my_num;
            my_public_key1 = my_public_key;
            PORT1 = PORT;
            token1 = token;
            InitializeComponent();
            Thread thread = new Thread(() => ListenOnPort(PORT));
            thread.Start();
            Username.Text=username;
        }
        public static void ListenOnPort(int port)
        {
            // Create a TCP listener on the specified port
            TcpListener listener = new TcpListener(IPAddress.Any, port);
            listener.Start();

            Console.WriteLine($"Listening on port {port}...");

            while (should_exist)
            {
                // Wait for a client to connect with a timeout
                if (listener.Pending())
                {
                    TcpClient client = listener.AcceptTcpClient();
                    Console.WriteLine($"Client connected from {client.Client.RemoteEndPoint}");

                    // Get the network stream for the client
                    NetworkStream stream = client.GetStream();

                    // Keep reading messages from the client and printing them
                    byte[] buffer = new byte[1024];
                    int bytesRead;
                    while ((bytesRead = stream.Read(buffer, 0, buffer.Length)) > 0 && should_exist)
                    {
                        // Convert the received bytes to a string and print it
                        string message1 = System.Text.Encoding.ASCII.GetString(buffer, 0, bytesRead);
                        string message = geter(message1);

                        if (message1[0] == 'b')
                            sign = "message sent successfully";
                        if (message1[0] == 'c')
                            sign = "token is not valid please log out";
                        if (message1[0] == 'f')
                            sign = "the target user does not exist";
                        if (message1[0] == 'r')
                        {
                            int start = message.IndexOf(":") + 1;
                            int end = message.IndexOf(",", start);
                            if (end < 0)
                                end = message.Length;
                            string reciver = message.Substring(start, end - start).Trim();
                            start = message.IndexOf("msg:") + +4;
                            end = message.Length - 15;
                            string delivered = message.Substring(start, end - start).Trim();
                            DateTime currentDateTime = DateTime.Now;

                            MainWindow.InsertDatainbox(my_conn, reciver, delivered, 1, currentDateTime);
                            string returning = "{\"msgid\":\"" + message.Substring(message.Length - 8, 7) + "\"}";
                            maker(returning, "k");
                        }
                        if (message1[0] == 'd')
                            sign = "user blocked successfully";
                        if (message1[0] == 'e')
                            sign = "user unblocked successfully";

                    }

                    // The client disconnected
                    Console.WriteLine($"Client disconnected from {client.Client.RemoteEndPoint}");

                    // Clean up the client resources
                    stream.Close();
                    client.Close();
                }
                else
                {
                    // Sleep for a short period to avoid excessive CPU usage
                    Thread.Sleep(100);
                }
            }

            listener.Stop();
        }
        static public void maker(string msg, string code)
        {
            string to_send = Get_Ready(Rout(2345), msg);
            string send_to_port = to_send.Substring(to_send.Length - 4, 4);
            to_send = to_send.Substring(0, to_send.Length - 4);
            string leng = to_send.Length.ToString();
            if (to_send.Length < 100)
            {
                leng = "0000" + leng;
            }
            else if (to_send.Length < 1000)
            {
                leng = "000" + leng;
            }
            else if (to_send.Length < 10000)
            {
                leng = "00" + leng;
            }
            else if (to_send.Length < 100000)
            {
                leng = "0" + leng;
            }
            to_send = code + leng + to_send;
            sender(Int32.Parse(send_to_port), to_send);
        }
        static public void sender(int port, string message)
        {
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), port);
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.Connect(endPoint);

            Console.WriteLine("Connected to server.");


            byte[] buffer = System.Text.Encoding.ASCII.GetBytes(message);
            socket.Send(buffer);
            Console.WriteLine("Message sent.");

            socket.Close();
        }
        public static int Encrypt(int message, int public_key, int n)
        {
            int e = public_key;
            int encrpyted_text = 1;
            while (e != 0)
            {
                encrpyted_text *= message;
                encrpyted_text %= n;
                e--;
            }
            return encrpyted_text;
        }
        public static IList<int> Encoder(string message, int public_key, int n)
        {
            IList<int> form = new List<int>();
            // calling the encrypting function in encoding function
            for (int i = 0; i < message.Length; i++)
                form.Add(Encrypt((int)message[i], public_key, n));
            return form;
        }
        public static string Get_Ready(IDictionary<string, string> guide, string msg)
        {
            foreach (var i in guide)
            {
                int pk = (i.Value[0] - 48) * 10 + i.Value[1] - 48;
                int n = (i.Value[2] - 48) * 100000 + (i.Value[3] - 48) * 10000 + (i.Value[4] - 48) * 1000 + (i.Value[5] - 48) * 100 + (i.Value[6] - 48) * 10 + i.Value[7] - 48;
                IList<int> mimi = new List<int>();
                mimi = Encoder(msg, pk, n);
                msg = null;
                foreach (var a in mimi)
                    msg = msg + a.ToString() + ",";
                msg = msg.Substring(0, msg.Length - 1);
                msg = msg + i.Key;


            }
            return msg;
        }
        public static IDictionary<string, string> Rout(int port)
        {
            TcpClient client = new TcpClient();
            IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 2345);
            client.Connect(serverEndPoint);
            NetworkStream clientStream = client.GetStream();
            //clientStream.Write(buffer, 0, buffer.Length);
            //clientStream.Flush();

            byte[] buffer = new byte[1024];

            string all = "c";
            string tosend = all;
            //buffer= ASCIIEncoding().GetBytes(tosend);
            buffer = Encoding.ASCII.GetBytes(tosend);
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();
            buffer = new byte[4096];
            int bytesRead = clientStream.Read(buffer, 0, 4096);
            IDictionary<string, string> routing = new Dictionary<string, string>();

            string str = Encoding.Default.GetString(buffer);

            routing.Add(str.Substring(0, 4), str.Substring(4, 8));
            routing.Add(str.Substring(12, 4), str.Substring(16, 8));
            routing.Add(str.Substring(24, 4), str.Substring(28, 8));
            routing.Add(str.Substring(36, 4), str.Substring(40, 8));

            return routing;
        }
        static public string geter(string status)
        {
            IList<int> form = new List<int>();
            int minw = 0;
            for (int i = 7; i < status.Length; i++)
            {
                if (status[i] == ',')
                {
                    form.Add(minw);
                    minw = 0;
                }
                else
                {
                    minw = minw * 10;
                    minw = minw + (status[i]) - 48;
                }
            }
            form.Add(minw);
            string new_msg = Decoder(form, my_public_key1, my_num1);
            return new_msg;
        }
        public static int Decrypt(int message, int public_key, int n)
        {
            int d = my_private_key1;
            int decrypted = 1;
            while (d != 0)
            {
                decrypted *= message;
                decrypted %= n;
                d--;
            }
            return decrypted;
        }
        static int GCD(int num1, int num2)
        {
            int Remainder;

            while (num2 != 0)
            {
                Remainder = num1 % num2;
                num1 = num2;
                num2 = Remainder;
            }

            return num1;
        }
        public static string Decoder(IList<int> encoded, int public_key, int n)
        {
            string s = null;
            // calling the decrypting function decoding function
            foreach (var num in encoded)
                s = s + Decrypt(num, public_key, n).ToString();
            return s;
        }
        private void Inbox(object sender, RoutedEventArgs e)
        {

            should_exist = false;
            thread.Join();

            My_Inbox n = new My_Inbox(my_username, my_conn, my_private_key1, my_num1, my_public_key1, PORT1, token1);
            n.Show();
            should_exist = false;

            Close();
        }
        private void send(object sender, RoutedEventArgs e)
        {

            should_exist = false;
            thread.Join();

            My_Send n = new My_Send(my_username, my_conn, my_private_key1, my_num1, my_public_key1, PORT1, token1);
            n.Show();
            should_exist = false;

            Close();
        }
        private void all_mail(object sender, RoutedEventArgs e)
        {

            should_exist = false;
            thread.Join();

            My_All_Mail n = new My_All_Mail(my_username, my_conn, my_private_key1, my_num1, my_public_key1, PORT1, token1);
            n.Show();
            should_exist = false;

            Close();
        }
        private void Block(object sender, RoutedEventArgs e)
        {

            should_exist = false;
            thread.Join();

            block n = new block(my_username, my_conn, my_private_key1, my_num1, my_public_key1, PORT1, token1);
            n.Show();
            should_exist = false;

            Close();
        }
        private void logout(object sender, RoutedEventArgs e)
        {

            string msg ="{\"username\":\"" + my_username + "\"}";
            maker(msg, "n");
            should_exist = false;
            thread.Join();

            Close();
        }
    }
}
