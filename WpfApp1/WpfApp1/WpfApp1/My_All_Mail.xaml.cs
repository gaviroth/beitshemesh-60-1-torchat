﻿using System;
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
using System.Data;

namespace WpfApp1
{
    /// <summary>
    /// Interaction logic for My_All_Mail.xaml
    /// </summary>
    public partial class My_All_Mail : Window
    {
        string my_username;
        SQLiteConnection my_conn;
        public My_All_Mail(string username, SQLiteConnection conn)
        {
            my_conn = conn;
            my_username = username;
            InitializeComponent();
            SQLiteCommand command = new SQLiteCommand("SELECT * FROM Messages", my_conn);

            // Create a new SQLiteDataAdapter and fill a DataSet with the results of the query
            SQLiteDataAdapter dataAdapter = new SQLiteDataAdapter(command);
            DataSet dataSet = new DataSet();
            dataAdapter.Fill(dataSet);

            // Set the ItemsSource of the DataGrid to the DataSet's default DataView
            dataGrid.ItemsSource = dataSet.Tables[0].DefaultView;

            // Add the DataGrid to the Window's content
            this.Content = dataGrid;
        }
        private void Inbox(object sender, RoutedEventArgs e)
        {
            My_Inbox n = new My_Inbox(my_username, my_conn);
            n.Show();
            Close();
        }
        private void logout(object sender, RoutedEventArgs e)
        {
            string msg = "{\"username\":\"" + my_username + "\"}"; ;
            MainWindow.maker(msg, "n");
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
    }
}