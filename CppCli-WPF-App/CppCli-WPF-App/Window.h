#ifndef WINDOW_H
#define WINDOW_H
///////////////////////////////////////////////////////////////////////////
// Window.h - C++\CLI implementation of WPF Application                  //
//          - Demo for CSE 687 Project #4                                //
// ver 2.0                                                               //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015 
//	Authour : Isira Samarasekera
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package demonstrates how to build a C++\CLI WPF application.  It 
*  provides one class, WPFCppCliDemo, derived from System::Windows::Window
*  that is compiled with the /clr option to run in the Common Language
*  Runtime, and another class MockChannel written in native C++ and compiled
*  as a DLL with no Common Language Runtime support.
*
*  The window class hosts, in its window, a tab control with three views, two
*  of which are provided with functionality that you may need for Project #4.
*  It loads the DLL holding MockChannel.  MockChannel hosts a send queue, a
*  receive queue, and a C++11 thread that reads from the send queue and writes
*  the deQ'd message to the receive queue.
*
*  The Client can post a message to the MockChannel's send queue.  It hosts
*  a receive thread that reads the receive queue and dispatches any message
*  read to a ListBox in the Client's FileList tab.  So this Demo simulates
*  operations you will need to execute for Project #4.
*
*  Required Files:
*  ---------------
*  Window.h, Window.cpp, MochChannel.h, MochChannel.cpp,
*  Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp
*
*  Build Command:
*  --------------
*  devenv CppCli-WPF-App.sln
*  - this builds C++\CLI client application and native mock channel DLL
*
*  Maintenance History:
*  --------------------
* version 3.0 28th April 2015
*	Completed for project 4
*  ver 2.0 : 15 Apr 2015
*  - completed message passing demo with moch channel
*  - added FileBrowserDialog to show files in a selected directory
*  ver 1.0 : 13 Apr 2015
*  - incomplete demo with GUI but not connected to mock channel

*/
/*
* Create C++/CLI Console Application
* Right-click project > Properties > Common Language Runtime Support > /clr
* Right-click project > Add > References
*   add references to :
*     System
*     System.Windows.Presentation
*     WindowsBase
*     PresentatioCore
*     PresentationFramework
*/
using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;
using namespace System::Xml::Linq;


#include "Model.h"
#include <iostream>

namespace CppCliWindows
{
  ref class WPFCppCliDemo : Window
  {

    // Controls for Window

    DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
    Grid^ hGrid = gcnew Grid();                    
    TabControl^ hTabControl = gcnew TabControl();
	TabItem^ hUploadTab = gcnew TabItem();
	TabItem^ hDownloadTab = gcnew TabItem();
	TabItem^ hFileSearchTab = gcnew TabItem();
	TabItem^ hTextSearchTab = gcnew TabItem();
	TabItem^ hPerformanceTab = gcnew TabItem();
    TabItem^ hSendMessageTab = gcnew TabItem();
    TabItem^ hFileListTab = gcnew TabItem();
    TabItem^ hConnectTab = gcnew TabItem();
    StatusBar^ hStatusBar = gcnew StatusBar();
    StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
    TextBlock^ hStatus = gcnew TextBlock();

	// Controls for File upload View

	Grid^ hUploadGrid = gcnew Grid();
	Grid^ hUploadSettingGrid = gcnew Grid();
	Button^ hUploadButton = gcnew Button();
	Label^ hUploadFilePathLabel = gcnew Label();
	TextBox^ hUploadFilePathTextbox = gcnew TextBox();
	Button^ hUploadBrowseButton = gcnew Button();
	Label^ hUploadEndpointLabel = gcnew Label();
	TextBox^ hUploadEndpointTextbox = gcnew TextBox();
	Label^ hUploadEndpointPortLabel = gcnew Label();
	TextBox^ hUploadENdpointPortTextbox = gcnew TextBox();
	Label^ hUploadRemoteFolderLabel = gcnew Label();
	TextBox^ hUploadRemoteFolderTextbox = gcnew TextBox();
	Button^ hUploadRemoteBrowseButton = gcnew Button();
	StackPanel^ hUploadStackPanel = gcnew StackPanel();

	// Controls for RemoteFolder View

	Grid^ hRemoteFolderListGrid = gcnew Grid();
	ListBox^ hRemoteFolderBox = gcnew ListBox();
	Grid^ hGrid3 = gcnew Grid();

	// Controls for File download View

	Grid^ hDownloadGrid = gcnew Grid();
	Grid^ hDownloadFolderGrid = gcnew Grid();
	Grid^ hDownloadSettingGrid = gcnew Grid();
	Button^ hDownloadButton = gcnew Button();
	Label^ hDownloadFilePathLabel = gcnew Label();
	TextBox^ hDownloadFilePathTextbox = gcnew TextBox();
	Button^ hRemoteFileBrowseForDownload = gcnew Button();
	Label^ hDownloadEndpointLabel = gcnew Label();
	TextBox^ hDownloadEndpointTextbox = gcnew TextBox();
	Label^ hDownloadEndpointPortLabel = gcnew Label();
	TextBox^ hDownloadENdpointPortTextbox = gcnew TextBox();
	StackPanel^ hDownloadStackPanel = gcnew StackPanel();
	Label^ hDownloadFolderPathLabel = gcnew Label();
	TextBox^ hDownloadFolderPathTextbox = gcnew TextBox();
	Button^ hDownloadFolderBrowseButton = gcnew Button();

	// Controls for Remote File View

	Grid^ hRemoteFileListGrid = gcnew Grid();
	ListBox^ hRemoteFileBox = gcnew ListBox();
	Grid^ hGrid4 = gcnew Grid();

	// Controls for File Search View

	Grid^ hFileSearchGrid = gcnew Grid();
	Label^ hFileSearchRemoteIpLabel = gcnew Label();
	TextBox^ hFileSearchRemoteIpTextbox = gcnew TextBox();
	Label^ hFileSearchRemotePortLabel = gcnew Label();
	TextBox^ hFileSearchRemotePortTextbox = gcnew TextBox();
	Button^ hRemoteFolderBrowseForFileSearch = gcnew Button();

	Label^ hFileSearchRootPathLabel = gcnew Label();
	TextBox^ hFileSearchRootPathTextbox = gcnew TextBox();
	Label^ hFilePatternsLabel = gcnew Label();
	TextBox^ hFilePatternsContentTextBox = gcnew TextBox();
	TextBox^ hFilePatternTextBox = gcnew TextBox();
	Button^ hFileAddPatternsButton = gcnew Button();
	CheckBox^ returnXmlString = gcnew CheckBox();

	Grid^ hRemoteFolderBoxToSelectRootGrid = gcnew Grid();
	ListBox^ hRemoteFolderBoxToSelectRoot = gcnew ListBox();

	Button^ hFileSearchButton = gcnew Button();
	ListBox^ hFileSearchResultsListView = gcnew ListBox();
	StackPanel^ hFileSearchStackPanel = gcnew StackPanel();

	// Controls for text Search View

	Grid^ hTextSearchGrid = gcnew Grid();
	Grid^ hTextSearchSettingGrid = gcnew Grid();
	Grid^ hFileSearchConfigurationProperties = gcnew Grid();
	Grid^ hFileSearchSettingGrid = gcnew Grid();
	Label^ hTextSearchRemoteIpLabel = gcnew Label();
	TextBox^ hTextSearchRemoteIpTextbox = gcnew TextBox();
	Label^ hTextSearchRemotePortLabel = gcnew Label();
	TextBox^ hTextSearchRemotePortTextbox = gcnew TextBox();
	Button^ hRemoteEndpointAddButton = gcnew Button(); 
	Label^ remoteEndpointsLabel = gcnew Label();
	TextBox^ remoteEndpointTextBox = gcnew TextBox();

	Label^ hTextSearchFilePatternsLabel = gcnew Label();
	Button^ hTextSearchAddFilePatternsButton = gcnew Button();
	TextBox^ hTextSearchFilePatternAddedTextBox = gcnew TextBox();
	TextBox^ hTextSearchFilePatternsContentTextBox = gcnew TextBox();

	Label^ hTextSearchSpecLabel = gcnew Label();
	TextBox^ hTextSearchSpecTextbox = gcnew TextBox();
	CheckBox^ regexSearch = gcnew CheckBox();

	Label^ hThreadCountLabel = gcnew Label();
	TextBox^ hThreadCountTextbox = gcnew TextBox();

	Button^ hTextSearchButton = gcnew Button();
	ListBox^ hTextSearchResultListBox = gcnew ListBox();
	StackPanel^ hTextSearchStackPanel = gcnew StackPanel();

	// Controls for Performace 
	Grid^ hPerformanceGrid = gcnew Grid();
	ListBox^ hPerformanceList = gcnew ListBox();
	StackPanel^ hPerformancePanel = gcnew StackPanel();

    // Controls for send message

    Grid^ hSendMessageGrid = gcnew Grid();
    Button^ hSendButton = gcnew Button();
    Button^ hClearButton = gcnew Button();
    TextBlock^ hTextBlock1 = gcnew TextBlock();
    ScrollViewer^ hScrollViewer1 = gcnew ScrollViewer();
	ListBox^ hFileSearchResultListBox = gcnew ListBox();
    StackPanel^ hStackPanel1 = gcnew StackPanel();

    // Controls for FileListView View

    Grid^ hFileListGrid = gcnew Grid();
    Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
	Forms::OpenFileDialog^ hFileBrowserDialog = gcnew Forms::OpenFileDialog();
    ListBox^ hListBox = gcnew ListBox();
    Button^ hFolderBrowseButton = gcnew Button();
    Grid^ hGrid2 = gcnew Grid();

   
	Model^ model;
	
  public:
    WPFCppCliDemo();
    ~WPFCppCliDemo();
	void addActionHandlers();
	void setLocalPort(size_t port);
	void init();

    void setUpStatusBar();
    void setUpTabControl();
	void setUpUploadView();
	void setUpRemoteFolderListView();

	void setUpDownloadView();
	void setRemoteFileListView();
	void setLocalDownloadDirectory();
	void setRowsAndColumnsForLocalDownloadLocalDirectory();

	void setUpFileSearchView();
	void setRootPathCandidateListView();
	void setSearchConfigurationProperties();
	void placeControlsInTextSearch();
	void setSearchConfigurationPropertiesRowsAndColumns();
	void setFileSearchButtonProperties();
	void setFileSearchResultsListView();

	void setUpTextSearchView();
	void setTextSearchRemoteConncetionProperties();
	void setControlThicknessForTextSearchView();
	void addControlsToTextSearchView();
	void addFirstTwoRowsControlsToTextSearchView();
	//setSearchConfigurationProperties();
	void setRowsAndColumnsForTextSearch();
	void setTextSearchButtonProperties();
	void setTextSearchResultsListView();
	void setTextSearchStackPanel();

	void setUpPerformanceResultsView();
	void setPerformanceResultsListView();
	void setPerformanceStackPanel();

    void setUpSendMessageView();
    void setUpFileListView();
    void setUpConnectionView();

	// receive thread
	void fileUploadMessageReceived(ServiceMessage& msg);
	void fileDownloadMessageReceived(ServiceMessage& msg);
	void pathExploreMessageReceived(ServiceMessage& msg);
	void directoryExploreMessageReceiver(ServiceMessage& msg);
	void fileSearchMessageReceiver(ServiceMessage& msg);
	void textSearchMessageReceived(ServiceMessage& msg);

    void sendMessage(Object^ obj, RoutedEventArgs^ args);
    void addText(String^ msg);
	void setStatusMessage(String^ msg);
	void addTextToRemoteFolderView(String^ strings);	
	void addTextToRemoteFileView(String^ strings);
	void addTextToRemoteFolderToSelectRootView(String^ strings);
	void addTextToFileSearchView(String^ strings);
	void addTextToTextSearchView(String^ strings);
	void addTextPerformanceView(String^ strings);
	void showEntireXmlSubtree(String^ strings);
	bool isReturnEntireXmlTree();
	void addTextToRemoteFolderTextBox(String^ strings);
	void addTextToRemoteFileTextBox(String^ strings);
	void addTextToRemoteFileSearchRootTextBox(String^ strings);	
	void addPatternToTextBox(String^ strings);
	void addPatternToTextSearchTextBox(String^ strings);
	void addEnpointToSearchText(String^ strings);
	void clearFileSearchResults();
	void clearTextSearchResults();
	void changeRemoteSettings(String^ strings);
    void clear(Object^ sender, RoutedEventArgs^ args);
    void browseForFolder(Object^ sender, RoutedEventArgs^ args);
	void browseForFileToUpload(Object^ sender, RoutedEventArgs^ args);
	void browseRemoteDirectory(Object^ sender, RoutedEventArgs^ args);
	void browseRemoteDirectoryForFileSearch(Object^ sender, RoutedEventArgs^ args);
	void remoteDirSelected(Object^ sender, SelectionChangedEventArgs^ args);
	void remoteFileSelected(Object^ sender, SelectionChangedEventArgs^ args);
	void remoteRootToSearchFilesSelected(Object^ sender, SelectionChangedEventArgs^ args);
	void addPattern(Object^ sender, RoutedEventArgs^ args);
	void addPatternForTextSearch(Object^ sender, RoutedEventArgs^ args);
	void addEndpointForTextSearch(Object^ sender, RoutedEventArgs^ args);
    void OnLoaded(Object^ sender, RoutedEventArgs^ args);
    void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);
	void updateRemoteFolderView(array<String^>^ dirs);

	void uploadFile(Object^ sender, RoutedEventArgs^ args);
	void downloadFile(Object^ sender, RoutedEventArgs^ args);
	void searchFiles(Object^ sender, RoutedEventArgs^ args);
	void searchText(Object^ sender, RoutedEventArgs^ args);

	void browseLocalFoldersToSelectDownloadDestination(Object^ sender, RoutedEventArgs^ args);
	void browseRemoteFilesToSelectDownloadSource(Object^ sender, RoutedEventArgs^ args);

	//ServiceMessage createExploreRemoteDirectoryMessage();
	//void setLocalPort(size_t port);

  private:
    std::string toStdString(String^ pStr);
    String^ toSystemString(std::string& str);

	void setUploadFilePathProperties();
	void setRowsAndColumnsForUploadFileView();
	void placeControlsInUploadView();
	void setUploadStackPanel();
	void setUploadButtonProperties();

	void setDownloadFilePathProperties();
	void placeControlsInDownloadView();
	void setRowsAndColumnsForDownloadFileView();
	void setDownloadStackPanel();
	void setDownloadButtonProperties();

	void setRemoteConncetionProperties();
	void setRemoteConnectionsRowsAndColumns();
	void setFileSearchStackPanel();

    void setTextBlockProperties();
    void setButtonsProperties();
  };
}


#endif
