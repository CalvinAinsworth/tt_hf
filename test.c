void test()
{
  vector<int> v = {1, 3, 5, 2, 6, 4, 8};
  
  sort(v.begin(), v.end(), greater<int>());
  
  for (int i=0; i<v.size(); i++)
    cout << v[i] << "";
  
}