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
using System.Text.Json;
using System.IO;
using System.Data.SQLite;

namespace WpfApp1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static public IList<int> primes = new List<int>();
        static public int my_private_key;
        static public int my_num;
        static public int my_public_key;
        static public int PORT;
        static public int token;
        static public string sign="";
        static public SQLiteConnection Connection;
        public static void ListenOnPort(int port)
        {
            // Create a TCP listener on the specified port
            TcpListener listener = new TcpListener(IPAddress.Any, port);
            listener.Start();

            Console.WriteLine($"Listening on port {port}...");

            while (true)
            {
                // Wait for a client to connect
                TcpClient client = listener.AcceptTcpClient();

                // Create a new thread to handle the client
                Thread clientThread = new Thread(() =>
                {
                    Console.WriteLine($"Client connected from {client.Client.RemoteEndPoint}");

                    // Get the network stream for the client
                    NetworkStream stream = client.GetStream();

                    // Keep reading messages from the client and printing them
                    while (true)
                    {
                        byte[] buffer = new byte[1024];
                        int bytesRead = stream.Read(buffer, 0, buffer.Length);
                        if (bytesRead == 0)
                        {
                            // The client disconnected
                            Console.WriteLine($"Client disconnected from {client.Client.RemoteEndPoint}");
                            break;
                        }

                        // Convert the received bytes to a string and print it
                        string message = geter(Encoding.UTF8.GetString(buffer, 0, bytesRead));
                        if (message[0] == 'b')
                            sign = "message sent successfully";
                        if (message[0] == 'c')
                            sign = "token is not valid please log out";
                        if (message[0] == 'f')
                            sign = "the target user does not exist";
                        if (message[0] == 'r')
                        {
                            int start = message.IndexOf(":") + 1;
                            int end = message.IndexOf(",", start);
                            if (end < 0)
                                end = message.Length;
                            string reciver=message.Substring(start, end - start).Trim();
                            start = message.IndexOf("msg:") + +4;
                            end = message.Length - 15;
                            string delivered= message.Substring(start, end - start).Trim();
                            DateTime currentDateTime = DateTime.Now;

                            InsertDatainbox(Connection, reciver, delivered, 1, currentDateTime);
                            string returning = "{\"msgid\":\"" + message.Substring(message.Length - 8, 7) + "\"}";
                            maker(returning, "k");
                        }
                        if (message[0] == 'd')
                            sign = "user blocked successfully";
                        if (message[0] == 'e')
                            sign = "user unblocked successfully";
                        Console.WriteLine($"Received message from {client.Client.RemoteEndPoint}: {message}");// insert this messege to the inbox table
                    }

                    // Clean up the client resources
                    stream.Close();
                    client.Close();
                });

                // Start the client thread and detach it
                clientThread.Start();
                clientThread.IsBackground = true;
            }
        }
        public static SQLiteConnection OpenDatabaseInDocuments(string databaseName)
        {
            // Get the user's document directory path
            string documentsPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);

            // Combine the document directory path with the database name
            string databasePath = System.IO.Path.Combine(documentsPath, databaseName);

            // Create a new SQLiteConnection object and open the database
            SQLiteConnection connection = new SQLiteConnection($"Data Source={databasePath}.db");
            connection.Open();

            // Return the connection object
            return connection;
        }
        public static SQLiteConnection Open(string databasePath)
        {
            SQLiteConnection connection = new SQLiteConnection($"Data Source=C:\\Documents\\{databasePath}.db");
            connection.Open();
            return connection;
        }
        public static SQLiteConnection CreateConnection(string databasePath)
        {

            SQLiteConnection sqlite_conn;
            // Create a new database connection:
            sqlite_conn = new SQLiteConnection($"Data Source=C:\\Documents\\{databasePath}.db; Version = 3; New = True; Compress = True; ");
            // Open the connection:
            try
            {
                sqlite_conn.Open();
            }
            catch (Exception ex)
            {

            }
            return sqlite_conn;
        }
        public static void CreateTable(SQLiteConnection conn)
        {

            SQLiteCommand sqlite_cmd;
            string createTableSql = "CREATE TABLE Messages (Id INTEGER PRIMARY KEY, Username TEXT, Message TEXT, IsSentByUser INTEGER, Timestamp DATETIME)";
            sqlite_cmd = conn.CreateCommand();

            sqlite_cmd.CommandText = createTableSql;
            sqlite_cmd.ExecuteNonQuery();

        }
        public static void CreateTableinbox(SQLiteConnection conn)
        {

            SQLiteCommand sqlite_cmd;
            string createTableSql = "CREATE TABLE Inbox (Id INTEGER PRIMARY KEY, Username TEXT, Message TEXT, IsSentByUser INTEGER, Timestamp DATETIME)";
            sqlite_cmd = conn.CreateCommand();

            sqlite_cmd.CommandText = createTableSql;
            sqlite_cmd.ExecuteNonQuery();

        }
        public static void CreateBlockedTable(SQLiteConnection connection)
        {
            try
            {
                // SQL command to create the "blocked" table
                string createTableCmd = "CREATE TABLE blocked (username TEXT)";

                // Create a new SQLite command object
                SQLiteCommand createTable = new SQLiteCommand(createTableCmd, connection);

                // Execute the SQL command to create the table
                createTable.ExecuteNonQuery();

            }
            catch (Exception ex)
            {
               
            }
        }
        public static List<string> GetBlockedUsernames(SQLiteConnection connection)
        {
            List<string> blockedUsernames = new List<string>();

            try
            {
                // SQL command to select all usernames from the "blocked" table
                string selectCmd = "SELECT username FROM blocked";

                // Create a new SQLite command object
                SQLiteCommand selectUsernames = new SQLiteCommand(selectCmd, connection);

                // Execute the SQL command to select all usernames from the "blocked" table
                SQLiteDataReader reader = selectUsernames.ExecuteReader();

                // Loop through the rows returned by the SQL query
                while (reader.Read())
                {
                    // Add each username to the list of blocked usernames
                    blockedUsernames.Add(reader.GetString(0));
                }

                reader.Close();

                Console.WriteLine("Blocked usernames retrieved successfully.");
            }
            catch (Exception ex)
            {
                // Error occurred while retrieving blocked usernames, handle the exception
                // You could also log the exception here for debugging purposes
                Console.WriteLine("Error retrieving blocked usernames: " + ex.Message);
            }

            return blockedUsernames;
        }
        public static void Blocking(string username,SQLiteConnection connection )
        {
            
                // Create a command to insert a new row into the blocked table
                using (SQLiteCommand command = new SQLiteCommand("INSERT INTO blocked (username) VALUES (@username)", connection))
                {
                    // Add the username parameter to the command
                    command.Parameters.AddWithValue("@username", username);

                    // Execute the command to insert the new row into the blocked table
                    command.ExecuteNonQuery();
                }

        }
        public static void unblocking(string username, SQLiteConnection connection)
        {
            // Open a connection to the database
            
                // Create a command to delete the row from the blocked table with the specified username
                using (SQLiteCommand command = new SQLiteCommand("DELETE FROM blocked WHERE username = @username", connection))
                {
                    // Add the username parameter to the command
                    command.Parameters.AddWithValue("@username", username);

                    // Execute the command to delete the row from the blocked table
                    command.ExecuteNonQuery();
                }
            
        }
        public static bool Block_check(SQLiteConnection connection,string username_to_check)
        {
            IList<string> stringList = new List<string>();
            stringList = GetBlockedUsernames(connection);
            foreach (string str in stringList)
            {
                // If the search string is found in the list, return true
                if (str.Equals(username_to_check))
                {
                    return true;
                }
            }

            // If the search string is not found in the list, return false
            return false;

        }
        public static void InsertData(SQLiteConnection conn, string username, string message, int IsSentByUser, DateTime timestamp)
        {
            SQLiteCommand sqlite_cmd;
            sqlite_cmd = conn.CreateCommand();
            string insertSql = "INSERT INTO Messages(Username, Message, IsSentByUser, Timestamp) VALUES(@Username, @Message, @IsSentByUser, @Timestamp)";
            using (SQLiteCommand command = new SQLiteCommand(insertSql, conn))
            {
                command.Parameters.AddWithValue("@Username", username);
                command.Parameters.AddWithValue("@Message", message);
                command.Parameters.AddWithValue("@IsSentByUser", IsSentByUser);
                command.Parameters.AddWithValue("@Timestamp", timestamp);

                command.ExecuteNonQuery();
            }



        }
        public static void InsertDatainbox(SQLiteConnection conn, string username, string message, int IsSentByUser, DateTime timestamp)
        {
            SQLiteCommand sqlite_cmd;
            sqlite_cmd = conn.CreateCommand();
            string insertSql = "INSERT INTO Inbox(Username, Message, IsSentByUser, Timestamp) VALUES(@Username, @Message, @IsSentByUser, @Timestamp)";
            using (SQLiteCommand command = new SQLiteCommand(insertSql, conn))
            {
                command.Parameters.AddWithValue("@Username", username);
                command.Parameters.AddWithValue("@Message", message);
                command.Parameters.AddWithValue("@IsSentByUser", IsSentByUser);
                command.Parameters.AddWithValue("@Timestamp", timestamp);

                command.ExecuteNonQuery();
            }



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
        static public string rec(int port)
        {
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Any, PORT);
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.Bind(endPoint);
            socket.Listen(10);

            Console.WriteLine("Server is running. Waiting for client...");

            Socket clientSocket = socket.Accept();
            Console.WriteLine("Client connected.");

            byte[] buffer = new byte[7];
            int bytesReceived = clientSocket.Receive(buffer);
            string message = System.Text.Encoding.ASCII.GetString(buffer, 0, bytesReceived);
            int len = int.Parse(message.Substring(1, 6));

            byte[] buffer1 = new byte[len];
            int bytesReceived1 = clientSocket.Receive(buffer1);
            string message1 = System.Text.Encoding.ASCII.GetString(buffer1, 0, bytesReceived1);

            Console.WriteLine("Received message: " + message);
            clientSocket.Close();
            socket.Close();
            return message+ message1;
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
        static public string Login(int port, string username, string password)
        {
            string sn = my_num.ToString();
            string pkn = my_public_key.ToString();
            while (sn.Length != 6)
                sn = "0" + sn;
            while (pkn.Length != 2)
                pkn = "0" + pkn;
            string json_str = "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}" + port + pkn + sn;
            string to_send = Get_Ready(Rout(port), json_str);
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
            to_send = "p" + leng + to_send;

            sender(Int32.Parse(send_to_port), to_send);
            return rec(port);
        }
        static public string signup(int port, string username, string password, string mail)
        {
            string sn = my_num.ToString();
            string pkn = my_public_key.ToString();
            while (sn.Length != 6)
                sn = "0" + sn;
            while (pkn.Length != 2)
                pkn = "0" + pkn;
            string json_str = "{\"Username\":\"" + username + "\",\"Password\":\"" + password + "\",\"Email\":\"" + mail + "\"}" + port + pkn + sn;
            string to_send = Get_Ready(Rout(port), json_str);
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
            to_send = "o" + leng + to_send;
            sender(Int32.Parse(send_to_port), to_send);

            return rec(port);
        }
        static public void maker(string msg,string code)
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
            string new_msg = Decoder(form, my_public_key, my_num);
            return new_msg;
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
            PORT = int.Parse(loginPort.Text);

            string status = Login(PORT, usernameBox.Text, PasswordBox.Text);
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
            string new_msg = Decoder(form, my_public_key, my_num);
            if (status[0] == 's')
            {
                SQLiteConnection sqlite_conn = OpenDatabaseInDocuments(usernameBox.Text);
                Connection = sqlite_conn;

                Thread thread = new Thread(() => ListenOnPort(PORT));
                thread.Start();
                token = int.Parse(new_msg.Substring(new_msg.Length - 7, 6));
                Connection = sqlite_conn;
                Menu menu = new Menu(usernameBox.Text,sqlite_conn);
                menu.Show();

                Close();
            }
            else
            {
                if(new_msg[0]=='g')
                    ErrorMessageTextBlock.Text = "password does not match";
                if (new_msg[0] == 'f')
                    ErrorMessageTextBlock.Text = "user does not exist please sign up";
            }
        }
        private void OnSignupButtonClick(object sender, RoutedEventArgs e)
        {
            PORT =int.Parse(signupPort.Text);

            string status = signup(PORT, SignupUsernameTextBox.Text, SignupPasswordBox.Text, SignupEmailTextBox.Text);
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
            string new_msg = Decoder(form, my_public_key, my_num);

            if (status[0] == 't')//DONT FORGET TO DECODE THIS
            {
                SQLiteConnection conn = OpenDatabaseInDocuments(SignupUsernameTextBox.Text);
                CreateBlockedTable(conn);
                CreateTable(conn);
                CreateTableinbox(conn);
                Connection = conn;

                Thread thread = new Thread(() => ListenOnPort(PORT));
                thread.Start();
                token = int.Parse(new_msg.Substring(new_msg.Length - 7, 6));
                Connection = conn;
                Menu menu = new Menu(SignupUsernameTextBox.Text,conn);
                menu.Show();
                Close();
            }
            else
            {
                if (new_msg[0] == 'j')
                    ErrorMessageTextBlock.Text = "email is not valid";
                if (new_msg[0] == 'i')
                    ErrorMessageTextBlock.Text = "password is not valid";
                if (new_msg[0] == 'h')
                    ErrorMessageTextBlock.Text = "username is already taken";
            }
        }
    }
}



