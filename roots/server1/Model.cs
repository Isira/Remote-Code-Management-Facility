using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// Sockets
using System.Net.Sockets;
using System.Net;

// observable collections
using System.Collections.ObjectModel;

// debug output
using System.Diagnostics;

// timer, sleep
using System.Threading;

using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows;

using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;

using System.Windows.Navigation;
using System.Windows.Shapes;

// hi res timer
using PrecisionTimers;

// Rectangle
// Must update References manually
using System.Drawing;

// INotifyPropertyChanged
using System.ComponentModel;

// byte data serialization
using System.Runtime.Serialization.Formatters.Binary;

// memory streams
using System.IO;

namespace BouncingBall
{
    public partial class Model : INotifyPropertyChanged
    {

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        //**************************************************
        //Creating Serializable Data to be sent over Sockets
        //***************************************************
        [Serializable]
        struct GameData
        {
            public bool leftKeyHit;
            public bool rightKeyHit;
            public double ballTop;
            public double ballLeft;
            public bool startHit;
            public bool restartHit;
            public GameData(bool l, bool r, double bT, double bL, bool s, bool rs)
            {
                leftKeyHit = l;
                rightKeyHit = r;
                ballTop = bT;
                ballLeft = bL;
                startHit = s;
                restartHit = rs;
            }
        }

        //************************************************************
        //Create UDP Sockets and Assign fields
        //*************************************************************
        UdpClient _dataSocket;

        // some data that keeps track of ports and addresses
        private static UInt32 _localPort;
        private static String _localIPAddress;
        private static UInt32 _remotePort;
        private static String _remoteIPAddress;
        //************************************************************
        //Create two threads for receiving the data and sync with 
        //************************************************************

        //this is the thread that will run in the background
        //waiting for incoming data
        private Thread _receiveDataThread;
        private Thread _sendDataThread;
        //this thread is used to synchronize the startup of two UDP peers
        private Thread _syncWithOtherPlayerThread;

        private static UInt32 _numBalls = 1;
        private UInt32[] _buttonPresses = new UInt32[_numBalls];
        Random _randomNumber = new Random();

        private TimerQueueTimer.WaitOrTimerDelegate _paddleTimerCallbackDelegate;

        private TimerQueueTimer _paddleHiResTimer;


        System.Drawing.Rectangle _paddlep1Rectangle;
        bool _movepaddlep1Left = false;
        bool _movepaddlep1Right = false;

        System.Drawing.Rectangle _paddlep2Rectangle;
        bool _movepaddlep2Left = false;
        bool _movepaddlep2Right = false;

        //Fields Required for the ball
        private TimerQueueTimer.WaitOrTimerDelegate _ballTimerCallbackDelegate;
        private TimerQueueTimer _ballHiResTimer;
        private double _ballXMove = 2;
        private double _ballYMove = 2;
        System.Drawing.Rectangle _ballRectangle;
        private bool _moveBall = false;

        public bool player2HitStart = false;
        public bool player2HitRestart = false;
        public int player1score = 0;
        public int player2score = 0;

        public bool MoveBall
        {
            get { return _moveBall; }
            set { _moveBall = value; }
        }

        private double _windowHeight = 100;
        public double WindowHeight
        {
            get { return _windowHeight; }
            set { _windowHeight = value; }
        }

        private double _windowWidth = 100;
        public double WindowWidth
        {
            get { return _windowWidth; }
            set { _windowWidth = value; }
        }

        /// <summary>
        /// Model constructor
        /// </summary>
        /// <returns></returns>
        public Model()
        {
            // this disables the Send button initially
            SendEnabled = false;

            // initialize the help test
            HelpText = "<-Select Socket Setup button to begin.";
        }

        //************************************************************
        //Define two functions for setting up the remote connection
        //************************************************************
        public void SetLocalNetworkSettings(UInt32 port, String ipAddress)
        {
            _localPort = port;
            _localIPAddress = ipAddress;
        }

        public void SetRemoteNetworkSettings(UInt32 port, String ipAddress)
        {
            _remotePort = port;
            _remoteIPAddress = ipAddress;
        }

        public bool InitModel()
        {

            //try
            //{
            //    // ***********************************************
            //    // set up generic UDP socket and bind to local port
            //    // ***********************************************
            //    _dataSocket = new UdpClient((int)_localPort);
            //}
            //catch (Exception ex)
            //{
            //    Debug.Write(ex.ToString());
            //    return false;
            //}

            // this delegate is needed for the multi media timer defined 
            // in the TimerQueueTimer class.
            _ballTimerCallbackDelegate = new TimerQueueTimer.WaitOrTimerDelegate(BallMMTimerCallback);

            // create our multi-media timers
            _ballHiResTimer = new TimerQueueTimer();
            try
            {
                // create a Multi Media Hi Res timer.
                _ballHiResTimer.Create(10, 10, _ballTimerCallbackDelegate);
            }
            catch (QueueTimerException ex)
            {
                Console.WriteLine(ex.ToString());
                Console.WriteLine("Failed to create Ball timer. Error from GetLastError = {0}", ex.Error);
            }

            _paddleTimerCallbackDelegate = new TimerQueueTimer.WaitOrTimerDelegate(paddleMMTimerCallback);
            _paddleHiResTimer = new TimerQueueTimer();
            try
            {
                // create a Multi Media Hi Res timer.
                _paddleHiResTimer.Create(10, 8, _paddleTimerCallbackDelegate);
            }
            catch (QueueTimerException ex)
            {
                Console.WriteLine(ex.ToString());
                Console.WriteLine("Failed to create paddle timer. Error from GetLastError = {0}", ex.Error);
            }

            // reset help text
            //HelpText = "";
            ThreadStart threadFunction;
            threadFunction = new ThreadStart(SynchWithOtherPlayer);
            _syncWithOtherPlayerThread = new Thread(threadFunction);
            return true;
        }

        public void CleanUp()
        {
            _ballHiResTimer.Delete();
            _paddleHiResTimer.Delete();
        }

        public void SendMessage()
        {
            //**************************************************************
            // data structure used to communicate data with the other player
            //**************************************************************
            GameData gameData;


            // formatter used for serialization of data
            BinaryFormatter formatter = new BinaryFormatter();

            // stream needed for serialization
            MemoryStream stream = new MemoryStream();

            // Byte array needed to send data over a socket
            Byte[] sendBytes;


            // we make sure that the data for the paddle is in the correct format
            try
            {
                gameData.leftKeyHit = _movepaddlep1Left;
                gameData.rightKeyHit = _movepaddlep1Right;
                gameData.ballTop = BallCanvasTop;
                gameData.ballLeft = BallCanvasLeft;
                gameData.startHit = MoveBall;
                gameData.restartHit = MoveBall;
            }
            catch (System.Exception ex)
            {
                // we get here if the format of the data in the boxes was incorrect. Most likely the boxes we assumed
                // had integers in them had characters as well
                StatusTextBox = StatusTextBox + DateTime.Now + " Data not in correct format! Try again.\n";
                return;
            }

            //**************************************************************
            // serialize the gameData structure to a stream
            //**************************************************************
            formatter.Serialize(stream, gameData);

            //**************************************************************
            // retrieve a Byte array from the stream
            //**************************************************************
            sendBytes = stream.ToArray();

            //**************************************************************
            // send the serialized data
            //**************************************************************
            IPEndPoint remoteHost = new IPEndPoint(IPAddress.Parse(_remoteIPAddress), (int)_remotePort);
            try
            {
                _dataSocket.Send(sendBytes, sendBytes.Length, remoteHost);
            }
            catch (SocketException ex)
            {
                StatusTextBox = StatusTextBox + DateTime.Now + ":" + " ERROR: Message not sent!\n";
                return;
            }

            StatusTextBox = StatusTextBox + DateTime.Now + ":" + " Message sent successfully.\n";
        }

        public void SetStartPosition()
        {

            P1Score = "";
            P2Score = "";

            BallHeight = 25;
            BallWidth = 25;


            BallCanvasLeft = _windowWidth / 2 - BallWidth / 2;
            BallCanvasTop = _windowHeight / 2;

            _moveBall = false;

            //PLAYER 1 PADDLE
            paddlep1Width = 120;
            paddlep1Height = 10;
            paddlep1CanvasLeft = _windowWidth / 2 - paddlep1Width / 2;
            paddlep1CanvasTop = _windowHeight - paddlep1Height;
            _paddlep1Rectangle = new System.Drawing.Rectangle((int)paddlep1CanvasLeft, (int)paddlep1CanvasTop, (int)paddlep1Width, (int)paddlep1Height);

            //PLAYER 2 PADDLE
            paddlep2Width = 120;
            paddlep2Height = 10;
            paddlep2CanvasLeft = _windowWidth / 2 - paddlep2Width / 2;
            paddlep2CanvasTop = 0;
            _paddlep2Rectangle = new System.Drawing.Rectangle((int)paddlep2CanvasLeft, (int)paddlep2CanvasTop, (int)paddlep2Width, (int)paddlep2Height);
        }

        public void MoveLeft(bool move)
        {
            _movepaddlep1Left = move;
        }

        public void MoveRight(bool move)
        {
            _movepaddlep1Right = move;
        }


        private void BallMMTimerCallback(IntPtr pWhat, bool success)
        {

            if (!_moveBall)
                return;

            // start executing callback. this ensures we are synched correctly
            // if the form is abruptly closed
            // if this function returns false, we should exit the callback immediately
            // this means we did not get the mutex, and the timer is being deleted.
            if (!_ballHiResTimer.ExecutingCallback())
            {
                Console.WriteLine("Aborting timer callback.");
                return;
            }

            if (player2HitStart == true)
            {
                BallCanvasLeft -= _ballXMove;
                BallCanvasTop -= _ballYMove;
            }
            else
            {
                BallCanvasLeft += _ballXMove;
                BallCanvasTop += _ballYMove;
            }

            // check to see if ball has it the left or right side of the drawing element
            if ((BallCanvasLeft + BallWidth >= _windowWidth) ||
                (BallCanvasLeft <= 0))
                _ballXMove = -_ballXMove;


            // check to see if ball has hit the top; stop moving the ball
            if (BallCanvasTop <= 0)
                _moveBall = false;

            if (BallCanvasTop + BallWidth >= _windowHeight)
            {
                // we hit bottom, stop moving the ball
                _moveBall = false;
            }

            // see if we hit the P1 paddle
            _ballRectangle = new System.Drawing.Rectangle((int)BallCanvasLeft, (int)BallCanvasTop, (int)BallWidth, (int)BallHeight);
            if (_ballRectangle.IntersectsWith(_paddlep1Rectangle))
            {
                // hit paddle. reverse direction in Y direction
                _ballYMove = -_ballYMove;

                // move the ball away from the paddle so we don't intersect next time around and
                // get stick in a loop where the ball is bouncing repeatedly on the paddle
                BallCanvasTop += 2 * _ballYMove;

                // add move the ball in X some small random value so that ball is not traveling in the same 
                // pattern
                BallCanvasLeft += _randomNumber.Next(5);
            }

            // see if we hit the P2 paddle
            if (_ballRectangle.IntersectsWith(_paddlep2Rectangle))
            {
                // hit paddle. reverse direction in Y direction
                _ballYMove = -_ballYMove;

                // move the ball away from the paddle so we don't intersect next time around and
                // get stick in a loop where the ball is bouncing repeatedly on the paddle
                BallCanvasTop += 2 * _ballYMove;

                // add move the ball in X some small random value so that ball is not traveling in the same 
                // pattern
                BallCanvasLeft += _randomNumber.Next(5);
            }
            // done in callback. OK to delete timer
            _ballHiResTimer.DoneExecutingCallback();
        }

        private void paddleMMTimerCallback(IntPtr pWhat, bool success)
        {

            // start executing callback. this ensures we are synched correctly
            // if the form is abruptly closed
            // if this function returns false, we should exit the callback immediately
            // this means we did not get the mutex, and the timer is being deleted.
            if (!_paddleHiResTimer.ExecutingCallback())
            {
                Console.WriteLine("Aborting timer callback.");
                return;
            }

            if (_movepaddlep1Left && paddlep1CanvasLeft > 0)
                paddlep1CanvasLeft -= 10;
            else if (_movepaddlep1Right && paddlep1CanvasLeft < _windowWidth - paddlep1Width)
                paddlep1CanvasLeft += 10;

            if (_movepaddlep2Right && paddlep2CanvasLeft > 0)
                paddlep2CanvasLeft -= 10;
            else if (_movepaddlep2Left && paddlep2CanvasLeft < _windowWidth - paddlep2Width)
                paddlep2CanvasLeft += 10;

            _paddlep1Rectangle = new System.Drawing.Rectangle((int)paddlep1CanvasLeft, (int)paddlep1CanvasTop, (int)paddlep1Width, (int)paddlep1Height);
            _paddlep2Rectangle = new System.Drawing.Rectangle((int)paddlep2CanvasLeft, (int)paddlep2CanvasTop, (int)paddlep2Width, (int)paddlep2Height);

            // done in callback. OK to delete timer
            _paddleHiResTimer.DoneExecutingCallback();
        }

        /// <summary>
        /// called when the view is closing to ensure we clean up our socket
        /// if we don't, the application may hang on exit
        /// </summary>
        public void Model_Cleanup()
        {
            // important. Close socket or application will not exit correctly.
            if (_dataSocket != null) _dataSocket.Close();
        }


        // this is the thread that waits for incoming messages
        private void ReceiveThreadFunction()
        {
            //Setup Receive end point
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Any, 0);
            while (true)
            {
                try
                {
                    // wait for data
                    Byte[] receiveData = _dataSocket.Receive(ref endPoint);

                    // check to see if this is synchronization data 
                    // ignore it. we should not recieve any sychronization
                    // data here, because synchronization data should have 
                    // been consumed by the SynchWithOtherPlayer thread. but, 
                    // it is possible to get 1 last synchronization byte, which we
                    // want to ignore
                    if (receiveData.Length < 2)
                        continue;


                    // process and display data


                    GameData gameData;
                    BinaryFormatter formatter = new BinaryFormatter();
                    MemoryStream stream = new MemoryStream();

                    //***************************************************
                    // deserialize data back into our GameData structure
                    //***************************************************

                    stream = new System.IO.MemoryStream(receiveData);
                    gameData = (GameData)formatter.Deserialize(stream);

                    // update view data through our bound properties                    
                    _movepaddlep2Left = gameData.leftKeyHit;
                    _movepaddlep2Right = gameData.rightKeyHit;
                    BallCanvasTop = gameData.ballTop;
                    BallCanvasLeft = gameData.ballLeft;
                    player2HitStart = gameData.startHit;
                    player2HitRestart = gameData.rightKeyHit;

                    // update status window
                    StatusTextBox = StatusTextBox + DateTime.Now + ":" + " New message received.\n";

                }
                catch (SocketException ex)
                {
                    // got here because either the Receive failed, or more
                    // or more likely the socket was destroyed by 
                    // exiting from the JoystickPositionWindow form
                    Console.WriteLine(ex.ToString());
                    return;
                }
                catch (Exception ex)
                { }

            }
        }


        public bool startListeningThread()
        {

            try
            {
                // ***********************************************
                // set up generic UDP socket and bind to local port
                // ***********************************************
                _dataSocket = new UdpClient((int)_localPort);
            }
            catch (Exception ex)
            {
                Debug.Write(ex.ToString());
                return false;
            }
            //**********************************************************************
            //Create a thread that continiously runs to connect to the other player
            //*********************************************************************

            ThreadStart threadFunction;
            threadFunction = new ThreadStart(SynchWithOtherPlayer);
            _syncWithOtherPlayerThread = new Thread(threadFunction);

            StatusTextBox = StatusTextBox + DateTime.Now + ":" + " Waiting for other UDP peer to join.\n";
            _syncWithOtherPlayerThread.Start();

            return true;
        }

        //public void startListeningThread()
        //{

        //    try
        //    {
        //        // ***********************************************
        //        // set up generic UDP socket and bind to local port
        //        // ***********************************************
        //        _dataSocket = new UdpClient((int)_localPort);
        //    }
        //    catch (Exception ex)
        //    {
        //        Debug.Write(ex.ToString());
        //        return false;
        //    }
        //    //**********************************************************************
        //    //Create a thread that continiously runs to connect to the other player
        //    //*********************************************************************

        //    //ThreadStart threadFunction;
        //    //threadFunction = new ThreadStart(SynchWithOtherPlayer);
        //    //_syncWithOtherPlayerThread = new Thread(threadFunction);

        //    StatusTextBox = StatusTextBox + DateTime.Now + ":" + " Waiting for other UDP peer to join.\n";
        //    _syncWithOtherPlayerThread.Start();
        //}

        public void SynchWithOtherPlayer()
        {

            // set up socket for sending synch byte to UDP peer
            // we can't use the same socket (i.e. _dataSocket) in the same thread context in this manner
            // so we need to set up a separate socket here
            Byte[] data = new Byte[1];
            IPEndPoint endPointSend = new IPEndPoint(IPAddress.Parse(_remoteIPAddress), (int)_remotePort);
            IPEndPoint endPointRecieve = new IPEndPoint(IPAddress.Any, 0);

            UdpClient synchSocket = new UdpClient((int)_localPort + 10);

            // set timeout of receive to 1 second
            _dataSocket.Client.ReceiveTimeout = 1000;

            while (true)
            {
                try
                {
                    synchSocket.Send(data, data.Length, endPointSend);
                    _dataSocket.Receive(ref endPointRecieve);

                    // got something, so break out of loop
                    break;
                }
                catch (SocketException ex)
                {
                    // we get an exception of there was a timeout
                    // if we timed out, we just go back and try again
                    if (ex.ErrorCode == (int)SocketError.TimedOut)
                    {
                        Debug.Write(ex.ToString());
                    }
                    else
                    {
                        // we did not time out, but got a really bad 
                        // error
                        synchSocket.Close();
                        StatusTextBox = StatusTextBox + "Socket exception occurred. Unable to sync with other UDP peer.\n";
                        StatusTextBox = StatusTextBox + ex.ToString();
                        return;
                    }
                }
                catch (System.ObjectDisposedException ex)
                {
                    // something bad happened. close the socket and return
                    Console.WriteLine(ex.ToString());
                    synchSocket.Close();
                    StatusTextBox = StatusTextBox + "Error occurred. Unable to sync with other UDP peer.\n";
                    return;
                }

            }

            // send synch byte
            synchSocket.Send(data, data.Length, endPointSend);

            // close the socket we used to send periodic requests to player 2
            synchSocket.Close();

            // reset the timeout for the dataSocket to infinite
            // _dataSocket will be used to recieve data from other UDP peer
            _dataSocket.Client.ReceiveTimeout = 0;

            // start the thread to listen for data from other UDP peer
            ThreadStart threadFunction = new ThreadStart(ReceiveThreadFunction);
            _receiveDataThread = new Thread(threadFunction);
            _receiveDataThread.Start();

            //ThreadStart threadFunctionSend = new ThreadStart(SendMessage);
            //_sendDataThread = new Thread(threadFunctionSend);
            //_sendDataThread.Start();


            // got this far, so we received a response from player 2
            StatusTextBox = StatusTextBox + DateTime.Now + ":" + " Other UDP peer has joined the session.\n";
            P1Score = player1score.ToString();
            P2Score = player2score.ToString();
            HelpText = "Movement:<LEFT><RIGHT>";
            SendEnabled = true;
        }

    }
}
