# GroupingCollectionView
This is an implementation of  ICollectionView for Xaml GridView and ListView that automatically groups and sorts an ObservableCollection using bindable functions.  

### WORK IN PROGRESS!

You may have noticed a dependency on files located in the "winrt" folder.  This was an attempt at writing this component using C++/WinRT rather than the normally required C++/CX.  However, in order to write a WinRT Component (accessible to other projects) it had to be done in C++/CX and so I abandoned trying to do it all in C++/WinRT.  I didn't completely remove it because sometime soon (end of 2017 maybe?), a new compiler is going to be released that will allow writing components in C++/WinRT and I'd eventually like to move over to that.

Please excuse my poorly optimized code.  I am a C++ novice.  This was an exercise.  

### TO USE

You can add this almost the same way you would add a regular `CollectionViewSource`.  However, instead of binding `GridView`'s `ItemsSource` to the `View` of `CollectionViewSource`, bind the `GroupingCollectionView` directly to `ItemsSource`.  

#### Source Property
`GroupingCollectionView`'s `Source` property takes an *ungrouped* `ObservableCollection` of items.  The grouping is instead done by the `GroupingSelector` property.

#### GroupingSelector Property
In code-behind or via binding with a `Func<object,object>`, create a function that will generate your groups.  `GroupingCollectionView` will only recognize an `IEnumerable` of `IEnumerable`.  So you can't set a path within your collection to the grouped items.  However, you can easily use Linq's IGrouping interface to generate groups.  Cast each `object` into your specific class to access its properties and use the `GroupBy` method from Linq.  Be sure to copy the result into a List using the `ToList()` extension. 

```
ContactsCV.GroupingSelector = (object list) =>
            {
                return (list as IEnumerable<object>).Cast<Contact>().GroupBy(x => x.LastName.Substring(0, 1).ToUpper()).OrderBy(x => x.Key).ToList();
            };
```

#### ItemSortFunction Property

```
 ContactsCV.ItemSortFunction = (object a, object b) =>
            {
                return (a as Contact).FirstName.CompareTo((b as Contact).FirstName);
            };
```
