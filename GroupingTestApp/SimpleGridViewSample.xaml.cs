using GroupingTestApp.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace GroupingTestApp
{
    public sealed partial class SimpleGridViewSample : Page
    {
        private ObservableCollection<Contact> _collection;

        public SimpleGridViewSample()
        {
            this.InitializeComponent();


            ContactsCV.ItemSortFunction = (object a, object b) =>
            {
                return (a as Contact).FirstName.CompareTo((b as Contact).FirstName);
            };

            ContactsCV.GroupingSelector = (object list) =>
            {
                return (list as IEnumerable<object>).Cast<Contact>().GroupBy(x => x.LastName.Substring(0, 1).ToUpper()).OrderBy(x => x.Key).ToList();
            };

            

            _collection = Contact.GetContacts(3);
            //Contact.GetContactsGrouped(250);
            ContactsCV.Source = _collection;
            ////ContactsCVS.Source = Contact.GetContactsIGrouped(250);

            var i = 3;

        }
       

        private void ItemsPresenter_Drop(object sender, DragEventArgs e)
        {
            Debug.WriteLine("Dropped!");
        }

        private void ItemsPresenter_DragOver(object sender, DragEventArgs e)
        {
            Debug.WriteLine("drag over");
            e.AcceptedOperation = Windows.ApplicationModel.DataTransfer.DataPackageOperation.Copy;
            e.Handled = true;
        }

        bool isAscending = true;
        
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (isAscending)
            {
                ContactsCV.ItemSortFunction = (object a, object b) =>
                {
                    return (b as Contact).FirstName.CompareTo((a as Contact).FirstName);
                };
            }
            else
            {
                ContactsCV.ItemSortFunction = (object a, object b) =>
                {
                    return (a as Contact).FirstName.CompareTo((b as Contact).FirstName);
                };
            }
            isAscending = !isAscending;

        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            if (_collection.Count > 0)
            {
                var lastName = _collection[0].LastName;
                _collection.Add(new Contact() { FirstName = Contact.GenerateFirstName(), LastName = "Xavier" });
            }
            else
                _collection.Add(new Contact() { FirstName = Contact.GenerateFirstName(), LastName = Contact.GenerateLastName() });
        }

        private void GridView_ChoosingGroupHeaderContainer(ListViewBase sender, ChoosingGroupHeaderContainerEventArgs args)
        {
            //Debug.WriteLine(args.Group);
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            _collection.RemoveAt(0);
        }
    }
}
