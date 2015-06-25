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
// reference the SocketSetupWindow Dialogue
using SocketSetup;

namespace BouncingBall
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Model _model;


        public MainWindow()
        {
            InitializeComponent();

            _model = new Model();
            this.DataContext = _model;
            // make it so the user cannot resize the window
            this.ResizeMode = ResizeMode.NoResize;
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            // create an instance of our Model
            _model = new Model();
            _model.WindowHeight = BallCanvas.RenderSize.Height;
            _model.WindowWidth = BallCanvas.RenderSize.Width - 16;
            this.GameGrid.DataContext = _model;
            _model.InitModel();
            _model.SetStartPosition();

        }

        private void KeypadDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Left)
                _model.MoveLeft(true);
            else if (e.Key == Key.Right)
                _model.MoveRight(true);
            else if (e.Key == Key.S)
            {
                //_model.MoveBall = !_model.MoveBall;
                _model.MoveBall = true;
                
            }
            else if (e.Key == Key.R)
                _model.SetStartPosition();
        }

        private void KeypadUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Left)
                _model.MoveLeft(false);
            else if (e.Key == Key.Right)
                _model.MoveRight(false);
        }

        


        //private void OnClosing(object sender, System.ComponentModel.CancelEventArgs e)
        //{
        //    _model.Model_Cleanup();
        //}

        private void SocketSetup_Button_Click(object sender, RoutedEventArgs e)
        {
            //****************************************************
            // Open a new window for setting up socket connection
            //****************************************************
            SocketSetupWindow socketSetupWindow = new SocketSetupWindow();
            socketSetupWindow.ShowDialog();

            //***************************************************
            //Set a Title for the Window
            //***************************************************
            this.Title = socketSetupWindow.SocketData.LocalIPString + "@" + socketSetupWindow.SocketData.LocalPort.ToString();

            //**************************************************
            //Update Model with The SetUp Data
            //*************************************************
            _model.SetLocalNetworkSettings(socketSetupWindow.SocketData.LocalPort, socketSetupWindow.SocketData.LocalIPString);
            _model.SetRemoteNetworkSettings(socketSetupWindow.SocketData.RemotePort, socketSetupWindow.SocketData.RemoteIPString);

            //*************************************************
            //Initialize the Model
            //************************************************* 
            _model.startListeningThread();
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _model.Model_Cleanup();
        }

    }
}
