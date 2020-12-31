# currency-converter
Currency converter written in C++ using Qt Creator framework

## If you want to try and test the program without downloading the source code and compiling the project 
You should go to the link (https://github.com/danillucky1234/currency-converter/releases/tag/v1.0 ) and download the archive with the .exe file. In the archive there are also .dll files that help the program to work correctly. So if you want to send this converter to your friend or run the application without Qt Creator you need to keep all the dynamic libraries with the .exe file. This is one of the peculiarities of working with Qt projects ( at least the Community version which was used to create this project).  

## If you want to download the source code of a project, you have to do a few things
If you follow them all consistently, you shouldn't have any problems.
1.	In the cmd use command: 
```git clone https://github.com/danillucky1234/currency-converter.git```
2.	In the Qt Creator:  
File -> Open File or Project -> [Go to the folder where you downloaded the project] -> [Select all files except the img folder] -> Press the Open button and the project opens
3.	Compile and run the project
4.	Nothing will work for you, but don't worry, this is how it should be. The values will not change because you need to add two libraries to work with the network (send and receive requests). In addition, in the Application Output tab you will get an error like this:  
```qt.network.ssl: QSslSocket::connectToHostEncrypted: TLS initialization failed```  
You have the libraries we need in the folder with all the .cpp and .h files.
5.	Go to the folder where our project was downloaded. Go one directory above and find a folder like this: build-currency_converter-Desktop_Qt_5_15_2_MinGW_64_bit-Debug ( you may have a slightly different folder name, but that's okay)
6.	Open this folder and go to the next folder "Debug"
7.	In this folder add libraries "libcrypto-1_1-x64.dll" and "libssl-1_1-x64.dll" (these libraries were downloaded along with the project)
8.	Open the tab with Qt creator again and start the project
9.	All values change according to the rate on a particular day  
## Important information:  
The free API from https://www.currencyconverterapi.com/ was used. But since it's free, there are a number of limitations:  
*	No more than 100 requests per hour
*	Date range, which currency rates you want to see - 8 days
*	Allowed Back in History- 1 year  
All of these limitations are considered in our program. When writing the code for this project used one API, but so you do not have to register on the site, you can use the API, which I registered especially for you (it is already installed in the code of the project, you can install the project and safely start).  
Of course you can go to the site, get your personal API and use it. If you get your API, you need to replace our public API( 45690ddeb55b52ed309c), in mainwindow.cpp, with your private one on these lines of code: 94, 102, 331.

### TODO:
* Bypass the restriction that prohibits sending queries and receiving responses to date ranges of more than 8 days
