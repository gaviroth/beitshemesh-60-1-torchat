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
    /// Interaction logic for My_Send.xaml
    /// </summary>
    public partial class My_Send : Window
    {
        string my_username;
        SQLiteConnection my_conn;
        public My_Send(string username, SQLiteConnection conn)
        {
            my_conn = conn;
            my_username = username;
            InitializeComponent();
            Username.Text=username;
        }
        private void Inbox(object sender, RoutedEventArgs e)
        {
            My_Inbox n = new My_Inbox(my_username,my_conn);
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
            block n = new block(my_username, my_conn);
            n.Show();
            Close();
        }
        private void logout(object sender, RoutedEventArgs e)
        {
            string msg = "{\"username\":\"" + my_username + "\"}";
            MainWindow.maker(msg, "n");
            Close();
        }
        private void submit(object sender, RoutedEventArgs e)
        {
            DateTime currentDateTime = DateTime.Now;

            MainWindow.InsertData(my_conn, target.Text, untarget.Text, 0, currentDateTime);
            string msg = "{\"sendingUser\":\"" + my_username + "\", \"receivingUser\":\"" + target.Text + "\",\"token\":\"" + MainWindow.token.ToString() + "\",\"msg\":\"" + untarget.Text + "\"}";
            MainWindow.maker(msg, "q");
            Thread.Sleep(100);
            error.Text = MainWindow.sign;
            target.Text="";
            untarget.Text="";
        }
    }
}
