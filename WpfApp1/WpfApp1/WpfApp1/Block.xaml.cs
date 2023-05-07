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
    /// Interaction logic for Block.xaml
    /// </summary>
    public partial class block : Window
    {
        string my_username;
        SQLiteConnection my_conn;
        public block(string username, SQLiteConnection conn)
        {
            my_conn = conn;
            my_username = username;
            InitializeComponent();
            IList<string> users = new List<string>();
            users = MainWindow.GetBlockedUsernames(my_conn);
            string to_show = "";
            foreach (string str in users)
            {
                to_show = to_show + str + "\n";
                     
            }
            list.Text = to_show;
        }
        private void Inbox(object sender, RoutedEventArgs e)
        {
            My_Inbox n = new My_Inbox(my_username, my_conn);
            n.Show();
            Close();
        }
        private void send(object sender, RoutedEventArgs e)
        {
            My_Send n = new My_Send(my_username, my_conn);
            n.Show();
            Close();
        }
        private void all_mail(object sender, RoutedEventArgs e)
        {
            My_All_Mail n = new My_All_Mail(my_username, my_conn);
            n.Show();
            Close();
        }
        private void Block(object sender, RoutedEventArgs e)
        {
            block n = new block(my_username,my_conn);
            n.Show();
            Close();
        }
        private void submit(object sender, RoutedEventArgs e)
        {
            MainWindow.Blocking(target.Text, my_conn);
            string to_send= "{\"username\":\"" + my_username + "\",\"blockedUser\":\"" + target.Text + "\",\"token\":\"" + MainWindow.token.ToString() + "\"}";
            MainWindow.maker(to_send, "l");
            //send this to the server
            Thread.Sleep(100);
            error.Text = MainWindow.sign;
        }
        private void submit1(object sender, RoutedEventArgs e)
        {
            MainWindow.unblocking(untarget.Text, my_conn);
            string to_send = "{\"username\":\"" + my_username + "\",\"unblockedUser\":\"" + target.Text + "\",\"token\":\"" + MainWindow.token.ToString() + "\"}"; ;
            MainWindow.maker(to_send, "m");
            //send this to the server
            Thread.Sleep(100);
            error.Text = MainWindow.sign;
        }
        private void logout(object sender, RoutedEventArgs e)
        {
            string msg = "{\"username\":\"" + my_username + "\"}";
            MainWindow.maker(msg, "n");
            Close();
        }
    }
}
