///////////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application                //
//          - Demo for CSE 687 Project #4                                //
// ver 2.0                                                               //
// Source :Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015    
// Authour: Isira Samarasekera
//
///////////////////////////////////////////////////////////////////////////
/*
*  To run as a Windows Application:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*  To run as a Console Application:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
*/
#include "Window.h"
#include "Model.h"
using namespace CppCliWindows;

//----< initialize client side>-----------

void WPFCppCliDemo::init()
{
	model->startClientSideObjects();
}

//----< set local port >-----------

void WPFCppCliDemo::setLocalPort(size_t port)
{
	model->setLocalPort(port);
}

//----< WPFCppCliDemo constructor >-----------

WPFCppCliDemo::WPFCppCliDemo()
{
	model = gcnew Model();
	model->fileUploadMessageReceived += gcnew FileUploadMessageReceived(this, &WPFCppCliDemo::fileUploadMessageReceived);
	model->fileDownloadMessageReceived += gcnew FileDownloadMessageReceived(this, &WPFCppCliDemo::fileDownloadMessageReceived);
	model->pathExploreMessageReceived += gcnew PathExploreMessageReceived(this, &WPFCppCliDemo::pathExploreMessageReceived);
	model->pathExploreMessageReceived += gcnew PathExploreMessageReceived(this, &WPFCppCliDemo::directoryExploreMessageReceiver);
	model->pathExploreMessageReceived += gcnew PathExploreMessageReceived(this, &WPFCppCliDemo::fileSearchMessageReceiver);
	model->textSearchMessageReceived += gcnew TextSearchMessageReceived(this, &WPFCppCliDemo::textSearchMessageReceived);

	addActionHandlers();

  // set Window properties

  this->Title = "Remote Code Management Client";
  this->Width = 800;
  this->Height = 600;

  // attach dock panel to Window

  this->Content = hDockPanel;
  hDockPanel->Children->Add(hStatusBar);
  hDockPanel->SetDock(hStatusBar, Dock::Bottom);
  hDockPanel->Children->Add(hGrid);

  // setup Window controls and views

  setUpTabControl();
  setUpStatusBar();
  setUpUploadView();
  setUpDownloadView();
  setUpFileSearchView();
  setUpTextSearchView();
  setUpPerformanceResultsView();
}

//----< WPFCppCliDemo destructor >-----------

WPFCppCliDemo::~WPFCppCliDemo()
{
}

//----< add Action Handlers >-----------

void WPFCppCliDemo::addActionHandlers()
{
	this->Loaded +=
		gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);
	this->Closing +=
		gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);
	hSendButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendMessage);
	hClearButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clear);
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
	hUploadBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFileToUpload);
	hUploadRemoteBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseRemoteDirectory);
	hRemoteFolderBox->SelectionChanged += gcnew SelectionChangedEventHandler(this, &WPFCppCliDemo::remoteDirSelected);
	hUploadButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::uploadFile);
	hDownloadFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseLocalFoldersToSelectDownloadDestination);
	hRemoteFileBrowseForDownload->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseRemoteFilesToSelectDownloadSource);
	hRemoteFileBox->SelectionChanged += gcnew SelectionChangedEventHandler(this, &WPFCppCliDemo::remoteFileSelected);
	hDownloadButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::downloadFile);
	hRemoteFolderBrowseForFileSearch->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseRemoteDirectoryForFileSearch);
	hRemoteFolderBoxToSelectRoot->SelectionChanged += gcnew SelectionChangedEventHandler(this, &WPFCppCliDemo::remoteRootToSearchFilesSelected);
	hFileAddPatternsButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addPattern);
	hFileSearchButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::searchFiles);
	hTextSearchButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::searchText);
	hTextSearchAddFilePatternsButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addPatternForTextSearch);
	hRemoteEndpointAddButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addEndpointForTextSearch);
}

//----< Set up status bar >-----------

void WPFCppCliDemo::setUpStatusBar()
{
  hStatusBar->Items->Add(hStatusBarItem);
  hStatus->Text = "very important messages will appear here";
  //status->FontWeight = FontWeights::Bold;
  hStatusBarItem->Content = hStatus;
  hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

//----< Set up tab controls >-----------

void WPFCppCliDemo::setUpTabControl()
{
  hGrid->Children->Add(hTabControl);
  hUploadTab->Header = "File Upload";
  hDownloadTab->Header = "File Download";
  hFileSearchTab->Header = "File Search";
  hTextSearchTab->Header = "Text Search";
  hPerformanceTab->Header = "Performance Results";

  hTabControl->Items->Add(hUploadTab);
  hTabControl->Items->Add(hDownloadTab);
  hTabControl->Items->Add(hFileSearchTab);
  hTabControl->Items->Add(hTextSearchTab);
  hTabControl->Items->Add(hPerformanceTab);

}

//----< Set up  upload file path properties >-----------

void WPFCppCliDemo::setUploadFilePathProperties()
{
	setRowsAndColumnsForUploadFileView();

	hUploadFilePathLabel->Content = "Local File Path: ";
	hUploadFilePathLabel->HorizontalContentAlignment = System::Windows::HorizontalAlignment::Right;
	hUploadFilePathLabel->VerticalContentAlignment = System::Windows::VerticalAlignment::Center;
	hUploadFilePathTextbox->VerticalContentAlignment = System::Windows::VerticalAlignment::Center;

	hUploadFilePathTextbox->Text = "File Path";
	hUploadBrowseButton->Content ="Browse";
	hUploadEndpointLabel->Content = "Remote IP: ";
	hUploadEndpointLabel->HorizontalContentAlignment = System::Windows::HorizontalAlignment::Right;
	hUploadEndpointTextbox->Text = "localhost";
	hUploadEndpointTextbox->VerticalContentAlignment = System::Windows::VerticalAlignment::Center;
	hUploadEndpointPortLabel->Content = "Remote Port: ";
	hUploadEndpointPortLabel->HorizontalContentAlignment = System::Windows::HorizontalAlignment::Right;
	hUploadENdpointPortTextbox->Text = "9996";
	hUploadENdpointPortTextbox->VerticalContentAlignment = System::Windows::VerticalAlignment::Center;

	hUploadRemoteFolderLabel->Content = "Remote Folder: ";
	hUploadRemoteFolderLabel->HorizontalContentAlignment = System::Windows::HorizontalAlignment::Right;
	hUploadRemoteFolderTextbox->VerticalContentAlignment = System::Windows::VerticalAlignment::Center;
	hUploadRemoteBrowseButton->Content = "Browse";

	hUploadFilePathLabel->Margin = Thickness(1, 6, 1, 6);
	hUploadFilePathTextbox->Margin = Thickness(1, 6, 1, 6);
	hUploadBrowseButton->Margin = Thickness(1, 6, 1, 6);
	hUploadEndpointLabel->Margin = Thickness(1, 6, 1, 6);
	hUploadEndpointTextbox->Margin = Thickness(1, 6, 1, 6);
	hUploadEndpointPortLabel->Margin = Thickness(1, 6, 1, 6);
	hUploadENdpointPortTextbox->Margin = Thickness(1, 6, 1, 6);
	hUploadRemoteFolderLabel->Margin = Thickness(1, 6, 1, 6);
	hUploadRemoteFolderTextbox->Margin = Thickness(1, 6, 1, 6);
	hUploadRemoteBrowseButton->Margin = Thickness(1, 6, 1, 6);
	
	placeControlsInUploadView();
	hUploadStackPanel->Children->Add(hUploadSettingGrid);
}

//----< place components in Upload view >-----------

void WPFCppCliDemo::placeControlsInUploadView()
{
	hUploadSettingGrid->SetRow(hUploadFilePathLabel, 0);
	hUploadSettingGrid->SetColumn(hUploadFilePathLabel, 0);

	hUploadSettingGrid->SetRow(hUploadFilePathTextbox, 0);
	hUploadSettingGrid->SetColumn(hUploadFilePathTextbox, 1);
	hUploadSettingGrid->SetColumnSpan(hUploadFilePathTextbox, 3);

	hUploadSettingGrid->SetRow(hUploadBrowseButton, 0);
	hUploadSettingGrid->SetColumn(hUploadBrowseButton, 4);

	hUploadSettingGrid->SetRow(hUploadEndpointLabel, 1);
	hUploadSettingGrid->SetColumn(hUploadEndpointLabel, 0);

	hUploadSettingGrid->SetRow(hUploadEndpointTextbox, 1);
	hUploadSettingGrid->SetColumn(hUploadEndpointTextbox, 1);
	hUploadSettingGrid->SetColumnSpan(hUploadEndpointTextbox, 2);

	hUploadSettingGrid->SetRow(hUploadEndpointPortLabel, 1);
	hUploadSettingGrid->SetColumn(hUploadEndpointPortLabel, 3);

	hUploadSettingGrid->SetRow(hUploadENdpointPortTextbox, 1);
	hUploadSettingGrid->SetColumn(hUploadENdpointPortTextbox, 4);

	hUploadSettingGrid->SetRow(hUploadRemoteFolderLabel, 2);
	hUploadSettingGrid->SetColumn(hUploadRemoteFolderLabel, 0);

	hUploadSettingGrid->SetRow(hUploadRemoteFolderTextbox, 2);
	hUploadSettingGrid->SetColumn(hUploadRemoteFolderTextbox, 1);
	hUploadSettingGrid->SetColumnSpan(hUploadRemoteFolderTextbox, 3);

	hUploadSettingGrid->SetRow(hUploadRemoteBrowseButton, 2);
	hUploadSettingGrid->SetColumn(hUploadRemoteBrowseButton, 4);


	hUploadSettingGrid->Children->Add(hUploadFilePathLabel);
	hUploadSettingGrid->Children->Add(hUploadFilePathTextbox);
	hUploadSettingGrid->Children->Add(hUploadBrowseButton);
	hUploadSettingGrid->Children->Add(hUploadEndpointLabel);
	hUploadSettingGrid->Children->Add(hUploadEndpointTextbox);
	hUploadSettingGrid->Children->Add(hUploadEndpointPortLabel);
	hUploadSettingGrid->Children->Add(hUploadENdpointPortTextbox);
	hUploadSettingGrid->Children->Add(hUploadRemoteFolderLabel);
	hUploadSettingGrid->Children->Add(hUploadRemoteFolderTextbox);
	hUploadSettingGrid->Children->Add(hUploadRemoteBrowseButton); 
}

//----< set rows and columns in upload file view >-----------

void WPFCppCliDemo::setRowsAndColumnsForUploadFileView()
{
	ColumnDefinition^ c0 = gcnew ColumnDefinition();
	ColumnDefinition^ c1 = gcnew ColumnDefinition();
	ColumnDefinition^ c2 = gcnew ColumnDefinition();
	ColumnDefinition^ c3 = gcnew ColumnDefinition();
	ColumnDefinition^ c4 = gcnew ColumnDefinition();

	RowDefinition^ r1 = gcnew RowDefinition();
	RowDefinition^ r2 = gcnew RowDefinition();
	RowDefinition^ r3 = gcnew RowDefinition();

	c0->Width = GridLength(120);
	c1->Width = GridLength(120);
	c2->Width = GridLength(120);
	c3->Width = GridLength(120);
	c4->Width = GridLength(120);

	r1->Height = GridLength(40);
	r2->Height = GridLength(40);
	r3->Height = GridLength(40);

	hUploadSettingGrid->ColumnDefinitions->Add(c0);
	hUploadSettingGrid->ColumnDefinitions->Add(c1);
	hUploadSettingGrid->ColumnDefinitions->Add(c2);
	hUploadSettingGrid->ColumnDefinitions->Add(c3);
	hUploadSettingGrid->ColumnDefinitions->Add(c4);

	hUploadSettingGrid->RowDefinitions->Add(r1);
	hUploadSettingGrid->RowDefinitions->Add(r2);
	hUploadSettingGrid->RowDefinitions->Add(r3);
}

//----< set upload button properties >-----------

void WPFCppCliDemo::setUploadButtonProperties()
{
	hUploadButton->Content = "Upload File";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hUploadButton;
	hUploadStackPanel->Children->Add(hBorder2);
}

//----< set upload stack panel >-----------

void WPFCppCliDemo::setUploadStackPanel()
{
	hUploadStackPanel->Orientation = Orientation::Vertical;
	hUploadStackPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hUploadGrid->SetRow(hUploadStackPanel, 1);
	hUploadGrid->Children->Add(hUploadStackPanel);
}

//----< set download file path properties >-----------

void WPFCppCliDemo::setDownloadFilePathProperties()
{
	setRowsAndColumnsForDownloadFileView();
	
	hDownloadFilePathLabel->Content = "Source File: ";
	hDownloadFilePathTextbox->Text = "Source File";
	hRemoteFileBrowseForDownload->Content = "Browse";
	hDownloadEndpointLabel->Content = "Remote Ip: ";
	hDownloadEndpointTextbox->Text = "localhost";
	hDownloadEndpointPortLabel->Content = "Remote Port: ";
	hDownloadEndpointPortLabel->HorizontalContentAlignment = System::Windows::HorizontalAlignment::Right;
	hDownloadENdpointPortTextbox->Text = "9996";

	hDownloadFilePathLabel->Margin = Thickness(1, 1, 1, 1);
	hDownloadFilePathTextbox->Margin = Thickness(1, 1, 1, 1);
	hRemoteFileBrowseForDownload->Margin = Thickness(1, 1, 1, 1);
	hDownloadEndpointLabel->Margin = Thickness(1, 1, 1, 1);
	hDownloadEndpointTextbox->Margin = Thickness(1, 1, 1, 1);
	hDownloadEndpointPortLabel->Margin = Thickness(1, 1, 1, 1);
	hDownloadENdpointPortTextbox->Margin = Thickness(1, 1, 1, 1);

	placeControlsInDownloadView();
	hDownloadStackPanel->Children->Add(hDownloadSettingGrid);
}

//----< place Controls in download view >-----------

void WPFCppCliDemo::placeControlsInDownloadView()
{
	hDownloadSettingGrid->SetRow(hDownloadEndpointLabel, 0);
	hDownloadSettingGrid->SetColumn(hDownloadEndpointLabel, 0);

	hDownloadSettingGrid->SetRow(hDownloadEndpointTextbox, 0);
	hDownloadSettingGrid->SetColumn(hDownloadEndpointTextbox, 1);
	hDownloadSettingGrid->SetColumnSpan(hDownloadEndpointTextbox, 2);

	hDownloadSettingGrid->SetRow(hDownloadEndpointPortLabel, 0);
	hDownloadSettingGrid->SetColumn(hDownloadEndpointPortLabel, 3);

	hDownloadSettingGrid->SetRow(hDownloadENdpointPortTextbox, 0);
	hDownloadSettingGrid->SetColumn(hDownloadENdpointPortTextbox, 4);

	hDownloadSettingGrid->SetRow(hRemoteFileBrowseForDownload, 0);
	hDownloadSettingGrid->SetColumn(hRemoteFileBrowseForDownload, 5);

	hDownloadSettingGrid->SetRow(hDownloadFilePathLabel, 1);
	hDownloadSettingGrid->SetColumn(hDownloadFilePathLabel, 0);

	hDownloadSettingGrid->SetRow(hDownloadFilePathTextbox, 1);
	hDownloadSettingGrid->SetColumn(hDownloadFilePathTextbox, 1);
	hDownloadSettingGrid->SetColumnSpan(hDownloadFilePathTextbox, 4);

	hDownloadSettingGrid->Children->Add(hDownloadFilePathLabel);
	hDownloadSettingGrid->Children->Add(hDownloadFilePathTextbox);
	hDownloadSettingGrid->Children->Add(hRemoteFileBrowseForDownload);
	hDownloadSettingGrid->Children->Add(hDownloadEndpointLabel);
	hDownloadSettingGrid->Children->Add(hDownloadEndpointTextbox);
	hDownloadSettingGrid->Children->Add(hDownloadEndpointPortLabel);
	hDownloadSettingGrid->Children->Add(hDownloadENdpointPortTextbox);
}

//----< set rows and columns for download file view >-----------

void WPFCppCliDemo::setRowsAndColumnsForDownloadFileView()
{
	ColumnDefinition^ c0 = gcnew ColumnDefinition();
	ColumnDefinition^ c1 = gcnew ColumnDefinition();
	ColumnDefinition^ c2 = gcnew ColumnDefinition();
	ColumnDefinition^ c3 = gcnew ColumnDefinition();
	ColumnDefinition^ c4 = gcnew ColumnDefinition();
	ColumnDefinition^ c5 = gcnew ColumnDefinition();

	RowDefinition^ r1 = gcnew RowDefinition();
	RowDefinition^ r2 = gcnew RowDefinition();

	c0->Width = GridLength(120);
	c1->Width = GridLength(120);
	c2->Width = GridLength(120);
	c3->Width = GridLength(120);
	c4->Width = GridLength(120);
	c5->Width = GridLength(120);

	r1->Height = GridLength(30);
	r2->Height = GridLength(30);

	hDownloadSettingGrid->ColumnDefinitions->Add(c0);
	hDownloadSettingGrid->ColumnDefinitions->Add(c1);
	hDownloadSettingGrid->ColumnDefinitions->Add(c2);
	hDownloadSettingGrid->ColumnDefinitions->Add(c3);
	hDownloadSettingGrid->ColumnDefinitions->Add(c4);
	hDownloadSettingGrid->ColumnDefinitions->Add(c5);

	hDownloadSettingGrid->RowDefinitions->Add(r1);
	hDownloadSettingGrid->RowDefinitions->Add(r2);
}

//----< set download button properties >-----------

void WPFCppCliDemo::setDownloadButtonProperties()
{
	hDownloadButton->Content = "Download File";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hDownloadButton;
	hDownloadStackPanel->Children->Add(hBorder2);
}

//----< set download stack panel >-----------

void WPFCppCliDemo::setDownloadStackPanel()
{
	hDownloadStackPanel->Orientation = Orientation::Vertical;
	hDownloadStackPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hDownloadGrid->SetRow(hDownloadStackPanel, 1);
	hDownloadGrid->Children->Add(hDownloadStackPanel);
}

//----< set upload view >-----------

void WPFCppCliDemo::setUpUploadView()
{
	Console::Write("\n  setting up upload view");
	hUploadGrid->Margin = Thickness(20);
	hUploadTab->Content = hUploadGrid;
	hUploadGrid->Width = hUploadTab->Width;
	setUploadFilePathProperties();
	setUpRemoteFolderListView();
	setUploadButtonProperties();
	setUploadStackPanel();
}

//----< set up remote folder list view >-----------

void WPFCppCliDemo::setUpRemoteFolderListView()
{
	Console::Write("\n  setting up Remote Folder list view");

	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Height =300;
	hBorder1->Child = hRemoteFolderBox;
	hUploadStackPanel->Children->Add(hBorder1);
}

//----< set up download view >-----------

void WPFCppCliDemo::setUpDownloadView()
{
	Console::Write("\n  setting up download view");
	hDownloadGrid->Margin = Thickness(20);
	hDownloadTab->Content = hDownloadGrid; 
	hUploadGrid->Width = hDownloadTab->Width;
	setDownloadFilePathProperties();
	setRemoteFileListView();
	setLocalDownloadDirectory();
	setDownloadButtonProperties();
	setDownloadStackPanel();
}

//----< set remote file list view >-----------

void WPFCppCliDemo::setRemoteFileListView()
{
	Console::Write("\n  setting up Remote File list view");

	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Height = 300;
	hBorder1->Child = hRemoteFileBox;
	hDownloadStackPanel->Children->Add(hBorder1);
}

//----< set local download directory >-----------

void WPFCppCliDemo::setLocalDownloadDirectory()
{
	// Make a grid of one row 
	// set the column size as same as the above panel
	// add buttons
	setRowsAndColumnsForLocalDownloadLocalDirectory();

	hDownloadFolderPathLabel->Content = "Destination Folder: ";
	hDownloadFolderPathLabel->HorizontalContentAlignment = System::Windows::HorizontalAlignment::Right;

	hDownloadFolderPathTextbox->Text = "";

	hDownloadFolderBrowseButton->Content = "Browse";

	hDownloadFolderPathLabel->Margin = Thickness(1, 1, 1, 1);
	hDownloadFolderPathTextbox->Margin = Thickness(1, 1, 1, 1);
	hDownloadFolderBrowseButton->Margin = Thickness(1, 1, 1, 1);

	hDownloadFolderGrid->SetRow(hDownloadFolderPathLabel, 0);
	hDownloadFolderGrid->SetColumn(hDownloadFolderPathLabel, 0);

	hDownloadFolderGrid->SetRow(hDownloadFolderPathTextbox, 0);
	hDownloadFolderGrid->SetColumn(hDownloadFolderPathTextbox, 1);
	hDownloadFolderGrid->SetColumnSpan(hDownloadFolderPathTextbox, 4);

	hDownloadFolderGrid->SetRow(hDownloadFolderBrowseButton, 0);
	hDownloadFolderGrid->SetColumn(hDownloadFolderBrowseButton, 5);

	hDownloadFolderGrid->Children->Add(hDownloadFolderPathLabel);
	hDownloadFolderGrid->Children->Add(hDownloadFolderPathTextbox);
	hDownloadFolderGrid->Children->Add(hDownloadFolderBrowseButton);

	hDownloadStackPanel->Children->Add(hDownloadFolderGrid);
}

//----< set rows and columsn for local download direcory >-----------

void WPFCppCliDemo::setRowsAndColumnsForLocalDownloadLocalDirectory()
{
	ColumnDefinition^ c0 = gcnew ColumnDefinition();
	ColumnDefinition^ c1 = gcnew ColumnDefinition();
	ColumnDefinition^ c2 = gcnew ColumnDefinition();
	ColumnDefinition^ c3 = gcnew ColumnDefinition();
	ColumnDefinition^ c4 = gcnew ColumnDefinition();
	ColumnDefinition^ c5 = gcnew ColumnDefinition();

	RowDefinition^ r1 = gcnew RowDefinition();

	c0->Width = GridLength(120);
	c1->Width = GridLength(120);
	c2->Width = GridLength(120);
	c3->Width = GridLength(120);
	c4->Width = GridLength(120);
	c5->Width = GridLength(120);

	r1->Height = GridLength(30);

	hDownloadFolderGrid->ColumnDefinitions->Add(c0);
	hDownloadFolderGrid->ColumnDefinitions->Add(c1);
	hDownloadFolderGrid->ColumnDefinitions->Add(c2);
	hDownloadFolderGrid->ColumnDefinitions->Add(c3);
	hDownloadFolderGrid->ColumnDefinitions->Add(c4);
	hDownloadFolderGrid->ColumnDefinitions->Add(c5);

	hDownloadFolderGrid->RowDefinitions->Add(r1);
}


//----< set up file search view >-----------

void WPFCppCliDemo::setUpFileSearchView()
{
	Console::Write("\n  setting up file search view");
	hFileSearchGrid->Margin = Thickness(20);
	hFileSearchTab->Content = hFileSearchGrid;
	setRemoteConncetionProperties();
	setRootPathCandidateListView();
	setSearchConfigurationProperties();
	setFileSearchButtonProperties();
	setFileSearchResultsListView();
	setFileSearchStackPanel();
}

//----< set up remote connection properties >-----------
void WPFCppCliDemo::setRemoteConncetionProperties()
{
	
	setRemoteConnectionsRowsAndColumns();

	hFileSearchRemoteIpLabel->Content = "Remote Ip";
	hFileSearchRemoteIpTextbox->Text = "localhost";
	hFileSearchRemotePortLabel->Content = "Remote Port";
	hFileSearchRemotePortTextbox->Text = "9996";
	hRemoteFolderBrowseForFileSearch->Content = "Browse";
	hFileSearchRootPathLabel->Content = "Root Path";

	hFileSearchRemoteIpLabel->Margin = Thickness(1, 1, 1, 1);
	hFileSearchRemoteIpTextbox->Margin = Thickness(1, 1, 1, 1);
	hFileSearchRemotePortLabel->Margin = Thickness(1, 1, 1, 1);
	hRemoteFolderBrowseForFileSearch->Margin = Thickness(1, 1, 1, 1);
	hFileSearchRootPathLabel->Margin = Thickness(1, 1, 1, 1);

	hFileSearchSettingGrid->SetRow(hFileSearchRemoteIpLabel, 0);
	hFileSearchSettingGrid->SetColumn(hFileSearchRemoteIpLabel, 0);

	hFileSearchSettingGrid->SetRow(hFileSearchRemoteIpTextbox, 0);
	hFileSearchSettingGrid->SetColumn(hFileSearchRemoteIpTextbox, 1);
	hFileSearchSettingGrid->SetColumnSpan(hFileSearchRemoteIpTextbox, 2);

	hFileSearchSettingGrid->SetRow(hFileSearchRemotePortLabel, 0);
	hFileSearchSettingGrid->SetColumn(hFileSearchRemotePortLabel, 3);

	hFileSearchSettingGrid->SetRow(hFileSearchRemotePortTextbox, 0);
	hFileSearchSettingGrid->SetColumn(hFileSearchRemotePortTextbox, 4);

	hFileSearchSettingGrid->SetRow(hRemoteFolderBrowseForFileSearch, 0);
	hFileSearchSettingGrid->SetColumn(hRemoteFolderBrowseForFileSearch, 5);

	hFileSearchSettingGrid->Children->Add(hFileSearchRemoteIpLabel);
	hFileSearchSettingGrid->Children->Add(hFileSearchRemoteIpTextbox);
	hFileSearchSettingGrid->Children->Add(hFileSearchRemotePortLabel);
	hFileSearchSettingGrid->Children->Add(hFileSearchRemotePortTextbox);
	hFileSearchSettingGrid->Children->Add(hRemoteFolderBrowseForFileSearch);

	hFileSearchStackPanel->Children->Add(hFileSearchSettingGrid);
}

//----< set remote connections rows and columns >-----------

void WPFCppCliDemo::setRemoteConnectionsRowsAndColumns()
{
	ColumnDefinition^ c0 = gcnew ColumnDefinition();
	ColumnDefinition^ c1 = gcnew ColumnDefinition();
	ColumnDefinition^ c2 = gcnew ColumnDefinition();
	ColumnDefinition^ c3 = gcnew ColumnDefinition();
	ColumnDefinition^ c4 = gcnew ColumnDefinition();
	ColumnDefinition^ c5 = gcnew ColumnDefinition();

	RowDefinition^ r1 = gcnew RowDefinition();

	c0->Width = GridLength(120);
	c1->Width = GridLength(120);
	c2->Width = GridLength(120);
	c3->Width = GridLength(120);
	c4->Width = GridLength(120);
	c5->Width = GridLength(120);

	r1->Height = GridLength(30);

	hFileSearchSettingGrid->ColumnDefinitions->Add(c0);
	hFileSearchSettingGrid->ColumnDefinitions->Add(c1);
	hFileSearchSettingGrid->ColumnDefinitions->Add(c2);
	hFileSearchSettingGrid->ColumnDefinitions->Add(c3);
	hFileSearchSettingGrid->ColumnDefinitions->Add(c4);
	hFileSearchSettingGrid->ColumnDefinitions->Add(c5);

	hFileSearchSettingGrid->RowDefinitions->Add(r1);
}

//----< set root path candidate list view >-----------

void WPFCppCliDemo::setRootPathCandidateListView()
{
	Console::Write("\n  setting up Root Path Candidates File list view");

	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Height = 100;
	hBorder1->Child = hRemoteFolderBoxToSelectRoot;
	hFileSearchStackPanel->Children->Add(hBorder1);
}

//----< set search configuration properties >-----------

void WPFCppCliDemo::setSearchConfigurationProperties()
{
	
	setSearchConfigurationPropertiesRowsAndColumns();

	hFileSearchRootPathLabel->Content = "Root Path";
	hFileSearchRemoteIpTextbox->Text = "localhost";
	hFileSearchRemotePortTextbox->Text = "9996";
	hFilePatternsLabel->Content = "File Patterns";
	hFileAddPatternsButton->Content = "Add Pattern";
	hFilePatternsContentTextBox->IsReadOnly = true;
	hFilePatternsContentTextBox->Text = "*.*";
	hFilePatternsContentTextBox->FontSize = 15;
	returnXmlString->Content = "return entire directory subtree xml";
	returnXmlString->VerticalAlignment = System::Windows::VerticalAlignment::Center;

	hFileSearchRootPathLabel->Margin = Thickness(1, 1, 1, 1);
	hFileSearchRootPathTextbox->Margin = Thickness(1, 1, 1, 1);
	hFilePatternsLabel->Margin = Thickness(1, 1, 1, 1);
	hFilePatternsContentTextBox->Margin = Thickness(1, 1, 1, 1);
	hFilePatternTextBox->Margin = Thickness(1, 1, 1, 1);
	hFileAddPatternsButton->Margin = Thickness(1, 1, 1, 1);

	placeControlsInTextSearch();


	hFileSearchConfigurationProperties->Children->Add(hFileSearchRootPathLabel);
	hFileSearchConfigurationProperties->Children->Add(hFileSearchRootPathTextbox);
	hFileSearchConfigurationProperties->Children->Add(hFilePatternsLabel);
	hFileSearchConfigurationProperties->Children->Add(hFilePatternsContentTextBox);
	hFileSearchConfigurationProperties->Children->Add(hFilePatternTextBox);
	hFileSearchConfigurationProperties->Children->Add(hFileAddPatternsButton);
	hFileSearchConfigurationProperties->Children->Add(returnXmlString);

	hFileSearchStackPanel->Children->Add(hFileSearchConfigurationProperties);
}

//----< place controls in text search>-----------

void WPFCppCliDemo::placeControlsInTextSearch()
{
	hFileSearchConfigurationProperties->SetRow(hFileSearchRootPathLabel, 0);
	hFileSearchConfigurationProperties->SetColumn(hFileSearchRootPathLabel, 0);

	hFileSearchConfigurationProperties->SetRow(hFileSearchRootPathTextbox, 0);
	hFileSearchConfigurationProperties->SetColumn(hFileSearchRootPathTextbox, 1);
	hFileSearchConfigurationProperties->SetColumnSpan(hFileSearchRootPathTextbox, 4);

	hFileSearchConfigurationProperties->SetRow(hFileAddPatternsButton, 1);
	hFileSearchConfigurationProperties->SetColumn(hFileAddPatternsButton, 0);

	hFileSearchConfigurationProperties->SetRow(hFilePatternTextBox, 1);
	hFileSearchConfigurationProperties->SetColumn(hFilePatternTextBox, 1);

	hFileSearchConfigurationProperties->SetRow(hFilePatternsLabel, 1);
	hFileSearchConfigurationProperties->SetColumn(hFilePatternsLabel, 2);

	hFileSearchConfigurationProperties->SetRow(hFilePatternsContentTextBox, 1);
	hFileSearchConfigurationProperties->SetColumn(hFilePatternsContentTextBox, 3);

	hFileSearchConfigurationProperties->SetRow(returnXmlString, 1);
	hFileSearchConfigurationProperties->SetColumn(returnXmlString, 4);
	hFileSearchConfigurationProperties->SetColumnSpan(returnXmlString, 2);
}

//----< set Search Configuration Properties Rows And Columns >-----------

void WPFCppCliDemo::setSearchConfigurationPropertiesRowsAndColumns()
{
	ColumnDefinition^ c0 = gcnew ColumnDefinition();
	ColumnDefinition^ c1 = gcnew ColumnDefinition();
	ColumnDefinition^ c2 = gcnew ColumnDefinition();
	ColumnDefinition^ c3 = gcnew ColumnDefinition();
	ColumnDefinition^ c4 = gcnew ColumnDefinition();
	ColumnDefinition^ c5 = gcnew ColumnDefinition();

	RowDefinition^ r1 = gcnew RowDefinition();
	RowDefinition^ r2 = gcnew RowDefinition();

	c0->Width = GridLength(120);
	c1->Width = GridLength(120);
	c2->Width = GridLength(120);
	c3->Width = GridLength(120);
	c4->Width = GridLength(120);
	c5->Width = GridLength(120);

	r1->Height = GridLength(30);
	r2->Height = GridLength(30);

	hFileSearchConfigurationProperties->ColumnDefinitions->Add(c0);
	hFileSearchConfigurationProperties->ColumnDefinitions->Add(c1);
	hFileSearchConfigurationProperties->ColumnDefinitions->Add(c2);
	hFileSearchConfigurationProperties->ColumnDefinitions->Add(c3);
	hFileSearchConfigurationProperties->ColumnDefinitions->Add(c4);
	hFileSearchConfigurationProperties->ColumnDefinitions->Add(c5);

	hFileSearchConfigurationProperties->RowDefinitions->Add(r1);
	hFileSearchConfigurationProperties->RowDefinitions->Add(r2);
}


//----< set File Search Button Properties >-----------

void WPFCppCliDemo::setFileSearchButtonProperties()
{
	hFileSearchButton->Content = "Search Files";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hFileSearchButton;
	hFileSearchStackPanel->Children->Add(hBorder2);
}

//----< set File Search Results ListView >-----------

void WPFCppCliDemo::setFileSearchResultsListView()
{
	Console::Write("\n  setting up File Search Result list view");

	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Height = 250;
	hBorder1->Child = hFileSearchResultsListView;
	hFileSearchStackPanel->Children->Add(hBorder1);
}

//----< set File Search Stack Panel >-----------

void WPFCppCliDemo::setFileSearchStackPanel()
{
	hFileSearchStackPanel->Orientation = Orientation::Vertical;
	hFileSearchStackPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hFileSearchGrid->SetRow(hFileSearchStackPanel, 1);
	hFileSearchGrid->Children->Add(hFileSearchStackPanel);
}

//----< browse Local Folders To Select Download Destination >-----------

void WPFCppCliDemo::browseLocalFoldersToSelectDownloadDestination(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for folder";
	hListBox->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		hDownloadFolderPathTextbox->Text = path;
	}
}

//----< browse Remote Files To Select Download Source >-----------

void WPFCppCliDemo::browseRemoteFilesToSelectDownloadSource(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browse Remote files to download";
	hRemoteFileBox->Items->Clear();

	Console::Write("\n  Explore remote directory command made");
	std::string _ip = toStdString( hDownloadEndpointTextbox->Text);
	int port = Convert::ToInt32(hDownloadENdpointPortTextbox->Text);
	model->exploreRootDirectory(_ip,port);
}

//----< set Text Block Properties >-----------

void WPFCppCliDemo::setTextBlockProperties()
{
  RowDefinition^ hRow1Def = gcnew RowDefinition();
  hSendMessageGrid->RowDefinitions->Add(hRow1Def);
  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
  hBorder1->Child = hTextBlock1;
  hTextBlock1->Padding = Thickness(15);
  hTextBlock1->Text = "";
  hTextBlock1->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
  hTextBlock1->FontWeight = FontWeights::Bold;
  hTextBlock1->FontSize = 16;
  hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
  hScrollViewer1->Content = hBorder1;
  hSendMessageGrid->SetRow(hScrollViewer1, 0);
  hSendMessageGrid->Children->Add(hScrollViewer1);
}

//----< set Buttons Properties >-----------

void WPFCppCliDemo::setButtonsProperties()
{
  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hRow2Def->Height = GridLength(75);
  hSendMessageGrid->RowDefinitions->Add(hRow2Def);
  hSendButton->Content = "Send Message";
  Border^ hBorder2 = gcnew Border();
  hBorder2->Width = 120;
  hBorder2->Height = 30;
  hBorder2->BorderThickness = Thickness(1);
  hBorder2->BorderBrush = Brushes::Black;
  hClearButton->Content = "Clear";
  hBorder2->Child = hSendButton;
  Border^ hBorder3 = gcnew Border();
  hBorder3->Width = 120;
  hBorder3->Height = 30;
  hBorder3->BorderThickness = Thickness(1);
  hBorder3->BorderBrush = Brushes::Black;
  hBorder3->Child = hClearButton;
  hStackPanel1->Children->Add(hBorder2);
  TextBlock^ hSpacer = gcnew TextBlock();
  hSpacer->Width = 10;
  hStackPanel1->Children->Add(hSpacer);
  hStackPanel1->Children->Add(hBorder3);
  hStackPanel1->Orientation = Orientation::Horizontal;
  hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hSendMessageGrid->SetRow(hStackPanel1, 1);
  hSendMessageGrid->Children->Add(hStackPanel1);
}

void WPFCppCliDemo::setUpTextSearchView()
{
	Console::Write("\n  setting up text Search view");
	hTextSearchGrid->Margin = Thickness(20);
	hTextSearchTab->Content = hTextSearchGrid;

	setTextSearchRemoteConncetionProperties();
	//setSearchConfigurationProperties();
	setTextSearchButtonProperties();
	setTextSearchResultsListView();
	setTextSearchStackPanel();
}

void WPFCppCliDemo::setTextSearchRemoteConncetionProperties()
{
	setRowsAndColumnsForTextSearch();

	hTextSearchRemoteIpLabel->Content = "Remote Ip";
	hTextSearchRemoteIpTextbox->Text = "localhost";
	hTextSearchRemotePortLabel->Content = "Remote Port";
	hTextSearchRemotePortTextbox->Text = "9996";
	hRemoteEndpointAddButton->Content = "Add Endpoint(+)";
	remoteEndpointsLabel->Content = "Endpoints";
	remoteEndpointTextBox->IsReadOnly = true;

	hTextSearchFilePatternsLabel->Content = "File Patterns";
	hTextSearchAddFilePatternsButton->Content = "Add Pattern";
	hTextSearchFilePatternsContentTextBox->IsReadOnly = true;
	hTextSearchFilePatternsContentTextBox->Text = "*.*";
	hTextSearchFilePatternsContentTextBox->FontSize = 15;

	hTextSearchSpecLabel->Content = "Search Text";
	regexSearch->Content = "Regex Search";

	hThreadCountLabel->Content = "Number of Concurrent Threads";
	hThreadCountTextbox->Text = "1";
	setControlThicknessForTextSearchView();

	addControlsToTextSearchView();
	hTextSearchStackPanel->Children->Add(hTextSearchSettingGrid);
}


void WPFCppCliDemo::setRowsAndColumnsForTextSearch()
{
	ColumnDefinition^ c0 = gcnew ColumnDefinition();
	ColumnDefinition^ c1 = gcnew ColumnDefinition();
	ColumnDefinition^ c2 = gcnew ColumnDefinition();
	ColumnDefinition^ c3 = gcnew ColumnDefinition();
	ColumnDefinition^ c4 = gcnew ColumnDefinition();
	ColumnDefinition^ c5 = gcnew ColumnDefinition();

	RowDefinition^ r1 = gcnew RowDefinition();
	RowDefinition^ r2 = gcnew RowDefinition();
	RowDefinition^ r3 = gcnew RowDefinition();
	RowDefinition^ r4 = gcnew RowDefinition();
	RowDefinition^ r5 = gcnew RowDefinition();

	c0->Width = GridLength(120);
	c1->Width = GridLength(120);
	c2->Width = GridLength(120);
	c3->Width = GridLength(120);
	c4->Width = GridLength(120);
	c5->Width = GridLength(120);

	r1->Height = GridLength(30);
	r2->Height = GridLength(30);
	r3->Height = GridLength(30);
	r4->Height = GridLength(30);
	r5->Height = GridLength(30);

	hTextSearchSettingGrid->ColumnDefinitions->Add(c0);
	hTextSearchSettingGrid->ColumnDefinitions->Add(c1);
	hTextSearchSettingGrid->ColumnDefinitions->Add(c2);
	hTextSearchSettingGrid->ColumnDefinitions->Add(c3);
	hTextSearchSettingGrid->ColumnDefinitions->Add(c4);
	hTextSearchSettingGrid->ColumnDefinitions->Add(c5);

	hTextSearchSettingGrid->RowDefinitions->Add(r1);
	hTextSearchSettingGrid->RowDefinitions->Add(r2);
	hTextSearchSettingGrid->RowDefinitions->Add(r3);
	hTextSearchSettingGrid->RowDefinitions->Add(r4);
	hTextSearchSettingGrid->RowDefinitions->Add(r5);
}

void WPFCppCliDemo::setControlThicknessForTextSearchView()
{
	hFileSearchRemoteIpLabel->Margin = Thickness(1, 1, 1, 1);
	hFileSearchRemoteIpTextbox->Margin = Thickness(1, 1, 1, 1);
	hFileSearchRemotePortLabel->Margin = Thickness(1, 1, 1, 1);
	hTextSearchRemotePortTextbox->Margin = Thickness(1, 1, 1, 1);
	hRemoteEndpointAddButton->Margin = Thickness(1, 1, 1, 1);
	remoteEndpointsLabel->Margin = Thickness(1, 1, 1, 1);
	remoteEndpointTextBox->Margin = Thickness(1, 1, 1, 1);
	hTextSearchFilePatternsLabel->Margin = Thickness(1, 1, 1, 1);
	hTextSearchAddFilePatternsButton->Margin = Thickness(1, 1, 1, 1);
	hTextSearchFilePatternsContentTextBox->Margin = Thickness(1, 1, 1, 1);
	hTextSearchFilePatternAddedTextBox->Margin = Thickness(1, 1, 1, 1);
	hThreadCountLabel->Margin = Thickness(1, 1, 1, 1);

	hTextSearchSpecLabel->Margin = Thickness(1, 1, 1, 1);
	hTextSearchSpecTextbox->Margin = Thickness(1, 1, 1, 1);
	regexSearch->Margin = Thickness(1, 1, 1, 1);

	hRemoteFolderBrowseForFileSearch->Margin = Thickness(1, 1, 1, 1);
	hFileSearchRootPathLabel->Margin = Thickness(1, 1, 1, 1);

	hThreadCountLabel->Margin = Thickness(1, 1, 1, 1);
	hThreadCountTextbox->Margin = Thickness(1, 1, 1, 1);
}

void WPFCppCliDemo::addControlsToTextSearchView()
{
	addFirstTwoRowsControlsToTextSearchView();

	hTextSearchSettingGrid->SetRow(hTextSearchSpecLabel, 3);
	hTextSearchSettingGrid->SetColumn(hTextSearchSpecLabel, 0);

	hTextSearchSettingGrid->SetRow(hTextSearchSpecTextbox, 3);
	hTextSearchSettingGrid->SetColumn(hTextSearchSpecTextbox, 1);
	hTextSearchSettingGrid->SetColumnSpan(hTextSearchSpecTextbox, 4);

	hTextSearchSettingGrid->SetRow(regexSearch, 3);
	hTextSearchSettingGrid->SetColumn(regexSearch, 5);

	hTextSearchSettingGrid->SetRow(hThreadCountLabel, 4);
	hTextSearchSettingGrid->SetColumn(hThreadCountLabel, 0);
	hTextSearchSettingGrid->SetColumnSpan(hThreadCountLabel, 2);

	hTextSearchSettingGrid->SetRow(hThreadCountTextbox, 4);
	hTextSearchSettingGrid->SetColumn(hThreadCountTextbox, 3);

	hTextSearchSettingGrid->Children->Add(hTextSearchRemoteIpLabel);
	hTextSearchSettingGrid->Children->Add(hTextSearchRemoteIpTextbox);
	hTextSearchSettingGrid->Children->Add(hTextSearchRemotePortLabel);
	hTextSearchSettingGrid->Children->Add(hTextSearchRemotePortTextbox);
	hTextSearchSettingGrid->Children->Add(hRemoteEndpointAddButton);
	hTextSearchSettingGrid->Children->Add(remoteEndpointsLabel);
	hTextSearchSettingGrid->Children->Add(remoteEndpointTextBox);

	hTextSearchSettingGrid->Children->Add(hTextSearchAddFilePatternsButton);
	hTextSearchSettingGrid->Children->Add(hTextSearchFilePatternAddedTextBox);
	hTextSearchSettingGrid->Children->Add(hTextSearchFilePatternsLabel);
	hTextSearchSettingGrid->Children->Add(hTextSearchFilePatternsContentTextBox);

	hTextSearchSettingGrid->Children->Add(hTextSearchSpecLabel);
	hTextSearchSettingGrid->Children->Add(hTextSearchSpecTextbox);
	hTextSearchSettingGrid->Children->Add(regexSearch);

	hTextSearchSettingGrid->Children->Add(hThreadCountLabel);
	hTextSearchSettingGrid->Children->Add(hThreadCountTextbox);
}

void WPFCppCliDemo::addFirstTwoRowsControlsToTextSearchView()
{
	hTextSearchSettingGrid->SetRow(hTextSearchRemoteIpLabel, 0);
	hTextSearchSettingGrid->SetColumn(hTextSearchRemoteIpLabel, 0);

	hTextSearchSettingGrid->SetRow(hTextSearchRemoteIpTextbox, 0);
	hTextSearchSettingGrid->SetColumn(hTextSearchRemoteIpTextbox, 1);
	hTextSearchSettingGrid->SetColumnSpan(hTextSearchRemoteIpTextbox, 2);

	hTextSearchSettingGrid->SetRow(hTextSearchRemotePortLabel, 0);
	hTextSearchSettingGrid->SetColumn(hTextSearchRemotePortLabel, 3);

	hTextSearchSettingGrid->SetRow(hTextSearchRemotePortTextbox, 0);
	hTextSearchSettingGrid->SetColumn(hTextSearchRemotePortTextbox, 4);

	hTextSearchSettingGrid->SetRow(hRemoteEndpointAddButton, 0);
	hTextSearchSettingGrid->SetColumn(hRemoteEndpointAddButton, 5);

	hTextSearchSettingGrid->SetRow(remoteEndpointsLabel, 1);
	hTextSearchSettingGrid->SetColumn(remoteEndpointsLabel, 0);

	hTextSearchSettingGrid->SetRow(remoteEndpointTextBox, 1);
	hTextSearchSettingGrid->SetColumn(remoteEndpointTextBox, 1);
	hTextSearchSettingGrid->SetColumnSpan(remoteEndpointTextBox, 5);

	hTextSearchSettingGrid->SetRow(hTextSearchAddFilePatternsButton, 2);
	hTextSearchSettingGrid->SetColumn(hTextSearchAddFilePatternsButton, 0);

	hTextSearchSettingGrid->SetRow(hTextSearchFilePatternAddedTextBox, 2);
	hTextSearchSettingGrid->SetColumn(hTextSearchFilePatternAddedTextBox, 1);
	hTextSearchSettingGrid->SetColumnSpan(hTextSearchFilePatternAddedTextBox, 2);

	hTextSearchSettingGrid->SetRow(hTextSearchFilePatternsLabel, 2);
	hTextSearchSettingGrid->SetColumn(hTextSearchFilePatternsLabel, 3);

	hTextSearchSettingGrid->SetRow(hTextSearchFilePatternsContentTextBox, 2);
	hTextSearchSettingGrid->SetColumn(hTextSearchFilePatternsContentTextBox, 4);
	hTextSearchSettingGrid->SetColumnSpan(hTextSearchFilePatternsContentTextBox, 2);
}
void WPFCppCliDemo::setTextSearchButtonProperties()
{
	hTextSearchButton->Content = "Search";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hTextSearchButton;
	hTextSearchStackPanel->Children->Add(hBorder2);
}

void WPFCppCliDemo::setTextSearchResultsListView()
{
	Console::Write("\n  setting up Root Path Candidates File list view");

	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Height = 250;
	hBorder1->Child = hTextSearchResultListBox;
	hTextSearchStackPanel->Children->Add(hBorder1);
}

void WPFCppCliDemo::setTextSearchStackPanel()
{
	hTextSearchStackPanel->Orientation = Orientation::Vertical;
	hTextSearchStackPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hTextSearchGrid->SetRow(hTextSearchStackPanel, 1);
	hTextSearchGrid->Children->Add(hTextSearchStackPanel);
}

void WPFCppCliDemo::setUpPerformanceResultsView()
{
	Console::Write("\n  setting up Performance results view");
	hPerformanceGrid->Margin = Thickness(20);
	hPerformanceTab->Content = hPerformanceGrid;

	setPerformanceResultsListView();
	setPerformanceStackPanel();
}

void WPFCppCliDemo::setPerformanceResultsListView()
{
	Console::Write("\n  setting up performance results list view");

	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Height = 450;
	hBorder1->Width =700;
	hBorder1->Child = hPerformanceList;
	hPerformancePanel->Children->Add(hBorder1);

}

void WPFCppCliDemo::setPerformanceStackPanel()
{
	hPerformancePanel->Orientation = Orientation::Vertical;
	hPerformancePanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hPerformanceGrid->SetRow(hPerformancePanel, 1);
	hPerformanceGrid->Children->Add(hPerformancePanel);
}

void WPFCppCliDemo::setUpSendMessageView()
{
  Console::Write("\n  setting up sendMessage view");
  hSendMessageGrid->Margin = Thickness(20);
  hSendMessageTab->Content = hSendMessageGrid;

  setTextBlockProperties();
  setButtonsProperties();
}

std::string WPFCppCliDemo::toStdString(String^ pStr)
{
  std::string dst;
  for (int i = 0; i < pStr->Length; ++i)
    dst += (char)pStr[i];
  return dst;
}

void WPFCppCliDemo::sendMessage(Object^ obj, RoutedEventArgs^ args)
{

 MessageFactory factory;
 std::vector<std::string> headers = {};
 std::vector<std::pair<std::string, std::string> > attributes;
 attributes.push_back(std::make_pair("srcIp",""));
 ServiceMessage msg = factory.construct(attributes);
  //pSendr_->postMessage(msg);
  Console::Write("\n  sent message");
  hStatus->Text = "Sent message";
}

String^ WPFCppCliDemo::toSystemString(std::string& str)
{
  StringBuilder^ pStr = gcnew StringBuilder();
  for (size_t i = 0; i < str.size(); ++i)
    pStr->Append((Char)str[i]);
  return pStr->ToString();
}

void WPFCppCliDemo::addText(String^ msg)
{
  hTextBlock1->Text += msg + "\n\n";
}

void WPFCppCliDemo::setStatusMessage(String^ msg)
{
	hStatus->Text = msg;
	MessageBox::Show(this,msg);
}

void WPFCppCliDemo::addTextToRemoteFolderView(String^ strings)
{
	hRemoteFolderBox->Items->Add(strings);
}

void WPFCppCliDemo::addTextToRemoteFileView(String^ strings)
{
	hRemoteFileBox->Items->Add(strings);
}

void WPFCppCliDemo::addTextToRemoteFolderToSelectRootView(String^ strings)
{
	hRemoteFolderBoxToSelectRoot->Items->Add(strings);
}

void WPFCppCliDemo::addTextToFileSearchView(String^ strings)
{
	hFileSearchResultsListView->Items->Add(strings);
}

void WPFCppCliDemo::addTextToTextSearchView(String^ strings)
{
	hTextSearchResultListBox->Items->Add(strings);
}

void WPFCppCliDemo::addTextPerformanceView(String^ strings)
{
	hPerformanceList->Items->Add(strings);
}

void WPFCppCliDemo::showEntireXmlSubtree(String^ strings)
{
	if (isReturnEntireXmlTree())
	MessageBox::Show(this,strings);
}

bool WPFCppCliDemo::isReturnEntireXmlTree()
{
	return (bool)returnXmlString->IsChecked;
}

void WPFCppCliDemo::changeRemoteSettings(String^ port)
{
	hRemoteFolderBox->Items->Clear();
	hRemoteFileBox->Items->Clear();
	hRemoteFolderBoxToSelectRoot->Items->Clear();
	hUploadENdpointPortTextbox->Text = port;
	hDownloadENdpointPortTextbox->Text = port;
	hFileSearchRemotePortTextbox->Text = port;
}
void WPFCppCliDemo::addTextToRemoteFolderTextBox(String^ strings)
{
	hUploadRemoteFolderTextbox->Text = strings;
}

void WPFCppCliDemo::addTextToRemoteFileTextBox(String^ strings)
{
	hDownloadFilePathTextbox->Text = strings;
}

void WPFCppCliDemo::addTextToRemoteFileSearchRootTextBox(String^ strings)
{
	hFileSearchRootPathTextbox->Text = strings;
}

void WPFCppCliDemo::addPatternToTextBox(String^ strings)
{
	if (strings == "")
		return;
	String^ patterns = hFilePatternsContentTextBox->Text;
	String^ modified;
	if (patterns->Equals("*.*"))
	{
		modified = strings;
	}
	else
	{
		modified = patterns + " , " + strings;
	}
	 
	hFilePatternsContentTextBox->Text = modified;
}

void WPFCppCliDemo::addPatternToTextSearchTextBox(String^ strings)
{
	if (strings == "")
		return;
	String^ patterns = hTextSearchFilePatternsContentTextBox->Text;
	String^ modified;
	if (patterns->Equals("*.*"))
	{
		modified = strings;
	}
	else
	{
		modified = patterns + " , " + strings;
	}

	hTextSearchFilePatternsContentTextBox->Text = modified;
}

void WPFCppCliDemo::addEnpointToSearchText(String^ strings)
{
	if (strings == "")
		return;
	String^ endpoint = remoteEndpointTextBox->Text;
	String^  modified;
	if (endpoint == "")
		modified = strings;
	else
		modified = endpoint + " , " + strings;
	remoteEndpointTextBox->Text = modified;
}

void WPFCppCliDemo::clearFileSearchResults()
{
	hFileSearchResultsListView->Items->Clear();
}

void WPFCppCliDemo::clearTextSearchResults()
{
	hTextSearchResultListBox->Items->Clear();
}

void WPFCppCliDemo::clear(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  cleared message text");
  hStatus->Text = "Cleared message";
  hTextBlock1->Text = "";
}

void WPFCppCliDemo::setUpFileListView()
{
  Console::Write("\n  setting up FileList view");
  hFileListGrid->Margin = Thickness(20);
  hFileListTab->Content = hFileListGrid;
  RowDefinition^ hRow1Def = gcnew RowDefinition();
  //hRow1Def->Height = GridLength(75);
  hFileListGrid->RowDefinitions->Add(hRow1Def);
  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
  hBorder1->Child = hListBox;
  hFileListGrid->SetRow(hBorder1, 0);
  hFileListGrid->Children->Add(hBorder1);

  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hRow2Def->Height = GridLength(75);
  hFileListGrid->RowDefinitions->Add(hRow2Def);
  hFolderBrowseButton->Content = "Select Directory";
  hFolderBrowseButton->Height = 30;
  hFolderBrowseButton->Width = 120;
  hFolderBrowseButton->BorderThickness = Thickness(2);
  hFolderBrowseButton->BorderBrush = Brushes::Black;
  hFileListGrid->SetRow(hFolderBrowseButton, 1);
  hFileListGrid->Children->Add(hFolderBrowseButton);

  hFolderBrowserDialog->ShowNewFolderButton = false;
  hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}

void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
  std::cout << "\n  Browsing for folder";
  hListBox->Items->Clear();
  System::Windows::Forms::DialogResult result;
  result = hFolderBrowserDialog->ShowDialog();
  if (result == System::Windows::Forms::DialogResult::OK)
  {
    String^ path = hFolderBrowserDialog->SelectedPath;
    std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
    array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
    for (int i = 0; i < files->Length; ++i)
      hListBox->Items->Add(files[i]);
    array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
    for (int i = 0; i < dirs->Length; ++i)
      hListBox->Items->Add(L"<> " + dirs[i]);
  }
}

void WPFCppCliDemo::browseForFileToUpload(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for file to upload";
	System::Windows::Forms::DialogResult result;
	result = hFileBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFileBrowserDialog->FileName;
		std::cout << "\n  opening file \"" << toStdString(path) << "\"";
		hUploadFilePathTextbox->Text = path;
	}
}

void WPFCppCliDemo::uploadFile(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Uploading file";

	std::string localFilePath = toStdString(hUploadFilePathTextbox->Text);
	std::string remoteFolderPath = toStdString(hUploadRemoteFolderTextbox->Text);

	int port = Convert::ToInt32(hUploadENdpointPortTextbox->Text);
	std::string ip = toStdString(hDownloadEndpointTextbox->Text);
	model->uploadFile(localFilePath, remoteFolderPath, ip,port);
}

void WPFCppCliDemo::downloadFile(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Downloading file";

	std::string remoteFilePath = toStdString(hDownloadFilePathTextbox->Text);
	std::string localFolderPath = toStdString(hDownloadFolderPathTextbox->Text);
	std::string ip = toStdString(hDownloadEndpointTextbox->Text);
	int port = Convert::ToInt32(hDownloadENdpointPortTextbox->Text);

	model->downloadFile(remoteFilePath, localFolderPath, ip,port);
}

void WPFCppCliDemo::searchFiles(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Searching files";

	std::string remoteRootPath = toStdString(hFileSearchRootPathTextbox->Text);
	std::string ip = toStdString(hFileSearchRemoteIpTextbox->Text);
	int port = Convert::ToInt32(hFileSearchRemotePortTextbox->Text);
	std::vector<std::string> patterns = model->parsePatternsFromString(toStdString(hFilePatternsContentTextBox->Text));
	model->exploreDirectory(remoteRootPath, ip, port, patterns);

	Action^ act = gcnew Action(this, &WPFCppCliDemo::clearFileSearchResults);
	Dispatcher->Invoke(act);  // must call addText on main UI thread
}


void WPFCppCliDemo::searchText(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Searching texts";


	std::vector<std::string> patterns = model->parsePatternsFromString(toStdString(hTextSearchFilePatternsContentTextBox->Text));
	std::string searchText = toStdString(hTextSearchSpecTextbox->Text);
	bool isRegex = (bool)regexSearch->IsChecked;
	int numThreads = std::stoi(toStdString(hThreadCountTextbox->Text));

	std::vector<std::pair<std::string, int>> endpoints = model->parseRemoteEndpointsFromString(toStdString(remoteEndpointTextBox->Text));
	
	for (unsigned int i = 0; i < endpoints.size(); i++)
	{
		model->searchText(patterns, searchText, endpoints[i].first, endpoints[i].second, isRegex, numThreads);
	}
	

	Action^ act = gcnew Action(this, &WPFCppCliDemo::clearTextSearchResults);
	Dispatcher->Invoke(act);  // must call addText on main UI thread
}

void WPFCppCliDemo::browseRemoteDirectory(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  Explore remote directory command made");
	std::string ip = toStdString(hUploadEndpointTextbox->Text);
	int port = Convert::ToInt32(hUploadENdpointPortTextbox->Text);
	model->exploreRootDirectory(ip,port);
}

void WPFCppCliDemo::browseRemoteDirectoryForFileSearch(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  Explore remote directory command made");
	std::string ip = toStdString(hFileSearchRemoteIpTextbox->Text);
	int port = Convert::ToInt32(hFileSearchRemotePortTextbox->Text);
	model->exploreRootDirectory(ip, port);
}

void WPFCppCliDemo::remoteDirSelected(Object^ sender, SelectionChangedEventArgs^ args)
{
	if (args->AddedItems->Count > 0)
	{
		String^ addedFolder = args->AddedItems[0]->ToString();
		array<String^>^ _args = gcnew array<String^>(1);
		_args[0] = addedFolder;
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextToRemoteFolderTextBox);
		Dispatcher->Invoke(act, _args);  // must call addText on main UI thread
	}
	
	
}

void WPFCppCliDemo::remoteFileSelected(Object^ sender, SelectionChangedEventArgs^ args)
{
	if (args->AddedItems->Count > 0)
	{
		String^ addedFolder = args->AddedItems[0]->ToString();
		array<String^>^ _args = gcnew array<String^>(1);
		_args[0] = addedFolder;
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextToRemoteFileTextBox);
		Dispatcher->Invoke(act, _args);  // must call addText on main UI thread
	}
}

void WPFCppCliDemo::remoteRootToSearchFilesSelected(Object^ sender, SelectionChangedEventArgs^ args)
{
	if (args->AddedItems->Count > 0)
	{
		String^ addedFolder = args->AddedItems[0]->ToString();
		array<String^>^ _args = gcnew array<String^>(1);
		_args[0] = addedFolder;
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextToRemoteFileSearchRootTextBox);
		Dispatcher->Invoke(act, _args);  // must call addText on main UI thread
	}
}

void WPFCppCliDemo::addPattern(Object^ sender, RoutedEventArgs^ args)
{
	String^ string = hFilePatternTextBox->Text;
	hFilePatternTextBox->Clear();
	array<String^>^ _args = gcnew array<String^>(1);
	_args[0] = string;
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addPatternToTextBox);
	Dispatcher->Invoke(act, _args);
}

void WPFCppCliDemo::addPatternForTextSearch(Object^ sender, RoutedEventArgs^ args)
{
	String^ string = hTextSearchFilePatternAddedTextBox->Text;
	hTextSearchFilePatternAddedTextBox->Clear();
	array<String^>^ _args = gcnew array<String^>(1);
	_args[0] = string;
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addPatternToTextSearchTextBox);
	Dispatcher->Invoke(act, _args);
}

void WPFCppCliDemo::addEndpointForTextSearch(Object^ sender, RoutedEventArgs^ args)
{
	String^ ip = hTextSearchRemoteIpTextbox->Text;
	String^ port = hTextSearchRemotePortTextbox->Text;

	if (ip==""|| port=="")
		return;

	hTextSearchRemoteIpTextbox->Clear();
	hTextSearchRemotePortTextbox->Clear();

	array<String^>^ _args = gcnew array<String^>(1);
	_args[0] = ip+"@"+port;
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addEnpointToSearchText);
	Dispatcher->Invoke(act, _args);
}

void WPFCppCliDemo::updateRemoteFolderView(array<String^>^ dirs)
{
	for (int i = 0; i < dirs->Length; i++)
	{
		array<String^>^ args = gcnew array<String^>(1);
		args[0] = dirs[i];
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextToRemoteFolderView);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
	}
	
}

void WPFCppCliDemo::setUpConnectionView()
{
  Console::Write("\n  setting up Connection view");
}

void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  Window loaded");
}
void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
  Console::Write("\n  Window closing");
  model->close();
}


void WPFCppCliDemo::fileUploadMessageReceived(ServiceMessage& msg)
{
	Console::Write("\n file Upload Message reply Received");
	if (msg.attribute("success") == "true")
	{
		std::string messageString = "File: " + msg.attribute("file") + " Successfully Uploaded to " + msg.attribute("dir");
		array<String^>^ args = gcnew array<String^>(1);
		args[0] = toSystemString(messageString);
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::setStatusMessage);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
	}

}

void WPFCppCliDemo::fileDownloadMessageReceived(ServiceMessage& msg)
{
	Console::Write("\n  file Download Message reply Received");
	std::string messageString = "File: " + msg.attribute("file") + " Successfully downloaded to " + msg.attribute("dir");
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = toSystemString(messageString);
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::setStatusMessage);
	Dispatcher->Invoke(act, args);  // must call addText on main UI thread

}

void WPFCppCliDemo::pathExploreMessageReceived(ServiceMessage& msg)
{
	std::string body = msg.body();

	std::vector<std::string> directories = model->getDirectoryPathsFromMessageBody(body);
	if (directories.empty() || directories[0] != "/")
		return;
		
	Console::Write("\n  explore root reply received");
	std::vector<std::string> files = model->getFilePathsFromMessageBody(body);

	array<String^>^ args = gcnew array<String^>(1);
	args[0] = toSystemString(msg.attribute("srcPort"));
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::changeRemoteSettings);
	Dispatcher->Invoke(act, args);  // must call addText on main UI thread

	
	for (unsigned int i = 0; i < directories.size(); i++)
	{
		array<String^>^ args = gcnew array<String^>(1);
		args[0] = toSystemString(directories[i]);
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextToRemoteFolderView);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
	}

	for (unsigned int i = 0; i < files.size(); i++)
	{
		array<String^>^ args = gcnew array<String^>(1);
		args[0] = toSystemString(files[i]);
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextToRemoteFileView);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
	}

	
}

void WPFCppCliDemo::directoryExploreMessageReceiver(ServiceMessage& msg)
{
	std::string body = msg.body();

	std::vector<std::string> directories = model->getDirectoryPathsFromMessageBody(body);
	if (directories.empty() || directories[0] != "/")
		return;

	for (unsigned int i = 0; i < directories.size(); i++)
	{
		array<String^>^ args = gcnew array<String^>(1);
		args[0] = toSystemString(directories[i]);
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextToRemoteFolderToSelectRootView);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
	}
}
void WPFCppCliDemo::fileSearchMessageReceiver(ServiceMessage& msg)
{
	std::string body = msg.body();

	std::vector<std::string> directories = model->getDirectoryPathsFromMessageBody(body);
	//std::string rootPath = toStdString(hFileSearchRootPathTextbox->Text);
	if (directories.empty() || directories[0] == "/")
		return;

	Console::Write("\n  file search reply received");
	std::vector<std::string> files = model->getFilePathsFromMessageBody(body);
	for (unsigned int i = 0; i < files.size(); i++)
	{
		array<String^>^ args = gcnew array<String^>(1);
		args[0] = toSystemString(files[i]);
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextToFileSearchView);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
	}

	array<String^>^ args = gcnew array<String^>(1);
	args[0] = toSystemString(body);
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::showEntireXmlSubtree);
	Dispatcher->Invoke(act,args);

	array<String^>^ __args = gcnew array<String^>(1);
	__args[0] = toSystemString(model->performanceDetailsForFileSearch(msg));
	Action<String^>^ __act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextPerformanceView);
	Dispatcher->Invoke(__act, __args);  // must call addText on main UI thread
}

void WPFCppCliDemo::textSearchMessageReceived(ServiceMessage& msg)
{
	Console::Write("\n  text Search Message reply Received");
	std::vector<std::string> files = model->getFilePathsWithEndpointFromMessage(msg);
	for (unsigned int i = 0; i < files.size(); i++)
	{
		array<String^>^ args = gcnew array<String^>(1);
		args[0] = toSystemString(files[i]);
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextToTextSearchView);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
	}
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = toSystemString(model->performanceDetailsForTextSearch(msg));
	Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addTextPerformanceView);
	Dispatcher->Invoke(act, args);  // must call addText on main UI thread
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
  Console::WriteLine(L"\n Starting WPFCppCliDemo");

  Application^ app = gcnew Application();
  WPFCppCliDemo^ window = gcnew WPFCppCliDemo();
  if (args->Length < 1)
  {
	  Console::WriteLine(L"Hasn't provided the port number.");
	  return 0;
  }
  int port = Convert::ToInt32(args[0]);
  window->setLocalPort(port);
  window->init();
  app->Run(window);
  Console::WriteLine(L"\n\n");
}