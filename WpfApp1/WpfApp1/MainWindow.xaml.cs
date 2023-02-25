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
namespace WpfApp1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static IList<int> primes = new List<int>();
        static int my_private_key;
        static int my_num;
        static int my_public_key;
        static int PORT;

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
        public static int Decrypt(int message, int public_key, int n)
        {
            int d = my_private_key;
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

        public static string Get_Ready(IDictionary<string, string> guide, string msg)
        {
            int bup = 0;
            foreach (var i in guide)
            {
                int pk = i.Value[0] - 48 * 10 + i.Value[1] - 48;
                int n = i.Value[2] - 48 * 100000 + i.Value[3] - 48 * 10000 + i.Value[4] - 48 * 1000 + i.Value[5] - 48 * 100 + i.Value[6] - 48 * 10 + i.Value[7] - 48;
                IList<int> mimi = new List<int>();
                mimi = Encoder(msg, pk, n);
                if (bup == 0)
                    msg = null;
                foreach (var a in mimi)
                    msg = msg + a.ToString() + ",";
                msg = msg.Substring(0, msg.Length - 1);
                msg = msg + i.Key;

                bup++;
            }
            return msg;
        }
        static void GetPrimesInRange()
        {

            for (int i = 2; i <= 250; i++)
            {
                bool isPrime = true;

                // check if i is divisible by any number between 2 and i - 1
                for (int j = 2; j < i; j++)
                {
                    if (i % j == 0)
                    {
                        isPrime = false;
                        break;
                    }
                }

                if (isPrime)
                {
                    primes.Add(i);
                }
            }

        }
        static int pickrandomprime()
        {
            Random rand = new Random();
            int i = primes.Count();
            int number = rand.Next(i);
            int num = primes[number];
            primes.Remove(number);
            return num;
        }
        static void setkeys()
        {
            int prime1 = pickrandomprime(); // first prime number
            int prime2 = pickrandomprime(); // second prime number
                                            // to check the prime numbers selected
                                            // cout<<prime1<<" "<<prime2<<endl;
            my_num = prime1 * prime2;
            int fi = (prime1 - 1) * (prime2 - 1);
            int e = 2;
            while (true)
            {
                if (GCD(e, fi) == 1)
                    break;
                e++;
            } // d = (k*Φ(n) + 1) / e for some integer k
            my_public_key = e;
            int d = 2;
            while (true)
            {
                if ((d * e) % fi == 1)
                    break;
                d++;
            }
            my_private_key = d;
        }
        static string rec(int port)
        {
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Any, PORT);
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.Bind(endPoint);
            socket.Listen(10);

            Console.WriteLine("Server is running. Waiting for client...");

            Socket clientSocket = socket.Accept();
            Console.WriteLine("Client connected.");

            byte[] buffer = new byte[1024];
            int bytesReceived = clientSocket.Receive(buffer);
            string message = System.Text.Encoding.ASCII.GetString(buffer, 0, bytesReceived);

            Console.WriteLine("Received message: " + message);
            clientSocket.Close();
            socket.Close();
            return message;
        }
        static void sender(int port, string message)
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
        static string Login(int port, string username, string password)
        {
            string json_str = "{'username':'" + username + "','password':'" + password + "}" + port + my_public_key.ToString() + my_num.ToString();
            string to_send = Get_Ready(Rout(port), json_str);
            string send_to_port = to_send.Substring(to_send.Length - 4, to_send.Length);
            to_send = to_send.Substring(0, to_send.Length - 4);
            string leng = to_send.Length.ToString();
            if (to_send.Length < 100)
            {
                leng = "00" + leng;
            }
            if (to_send.Length < 1000)
            {
                leng = "0" + leng;
            }
            to_send = "p" + leng + to_send;
            sender(Int32.Parse(send_to_port), to_send);
            return rec(port);
        }
        static string signup(int port, string username, string password, string mail)
        {
            string json_str = "{'username':'" + username + "','password':'" + password + "','Email':'" + mail + "}" + port + my_public_key.ToString() + my_num.ToString();
            string to_send = Get_Ready(Rout(port), json_str);
            string send_to_port = to_send.Substring(to_send.Length - 4, to_send.Length);
            to_send = to_send.Substring(0, to_send.Length - 4);
            string leng = to_send.Length.ToString();
            if (to_send.Length < 100)
            {
                leng = "00" + leng;
            }
            if (to_send.Length < 1000)
            {
                leng = "0" + leng;
            }
            to_send = "o" + leng + to_send;
            sender(Int32.Parse(send_to_port), to_send);

            return rec(port);
        }
        //get login handler
        //get database
        //

        public MainWindow()
        {
            InitializeComponent();
            PORT = 2365;
            GetPrimesInRange();
            setkeys();
        }
        private void OnLoginButtonClick(object sender, RoutedEventArgs e)
        {
            string status = Login(PORT, usernameBox.Text, PasswordBox.Text);
            if(status.Contains("successfully"))
                Close();
            else
            {
                ErrorMessageTextBlock.Text= status;
            }
        }
        private void OnSignupButtonClick(object sender, RoutedEventArgs e)
        {
            string status = signup(PORT, SignupUsernameTextBox.Text, SignupPasswordBox.Text, SignupEmailTextBox.Text);
            if (status.Contains("successfully"))//DONT FORGET TO DECODE THIS
                Close();
            else
            {
                ErrorMessageTextBlock.Text = status;
            }
        }
    }
}

