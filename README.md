# GroupingCollectionView
This is an implementation of  ICollectionView for Xaml GridView and ListView that automatically groups and sorts an ObservableCollection using bindable functions.  

# WORK IN PROGRESS!

You may have noticed a dependency on files located in the "winrt" folder.  This was an attempt at writing this component using C++/WinRT
rather than the normally required C++/CX.  However, in order to write a WinRT Component (accessible to other projects) it had to be done
in C++/CX and so I abandoned trying to do it all in C++/WinRT.  I didn't completely remove it because sometime soon (end of 2017 maybe?),
a new compiler is going to be released that will allow writing components in C++/WinRT and I'd eventually like to move over to that.

Please excuse my poorly optimized code.  I am a C++ novice.  This was an exercise.  
