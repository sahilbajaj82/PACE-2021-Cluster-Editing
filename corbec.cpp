#include<bits/stdc++.h>
using namespace std;

vector<vector<int>>adj;
int n, m;
vector<bool> visited;
vector<pair<int,int>>edge_set;
map<pair<int,int>,int>org;
time_t start, end1;

map<pair<int,int>,int>mod_edges;

volatile sig_atomic_t tle=0;
 
void timecheck() 
{
  time(&end1); 
  double time_taken = double(end1 - start);
  if(time_taken >= 540) {
    tle=1;
  }
}
int n_heur;
int conf;
int iter_bound;

void timecheck2() {
    iter_bound=INT_MAX;
}
struct comp 
{
  bool operator()(pair<int, int> const & a, pair<int, int> const & b) const 
  {
    if(a.first == b.first)  
    return a.second < b.second;
      
    return a.first > b.first;  
  }
};

map<pair<int, int>, int> weights;
map<pair<int, int>, int> orig_weights;

unordered_map<int, int> freq;
set<pair<int, int>, comp> order;
pair<int, int> best_edge_gl;

unordered_map<int, unordered_set<int>> freqEdges;
vector<vector<int>> adj_heur;

vector<bool> vis_res;



int get_hash(pair<int, int> p1)
{   
    if(p1.first == p1.second)
    return 0;

    int row = (p1.first < p1.second) ? p1.first : p1.second;
    int col = (p1.first < p1.second) ? p1.second : p1.first;
    
    return n_heur*(row-1) + (col-1);
}

pair<int, int> get_edge(int val1)
{
    int row = val1/n_heur;
    int col = val1 - row*n_heur;
    
    return {row+1, col+1};
}

void dfs_res(int node, vector<int> &comp)
{
  vis_res[node] = 1;
  comp.push_back(node);

  for(int child: adj_heur[node])
  {
    if(!vis_res[child])
    dfs_res(child, comp);
  }

}

void findbest()
{
    for(int i=1; i<=n_heur; ++i)
    { for(int j: adj_heur[i])
      { for(int k: adj_heur[j])
        {
          if(k <= i)
          continue;
          
          if(weights.find({i, k}) == weights.end())
          { 
            int val1 = get_hash({i, j});
            int val2 = get_hash({j, k});  
            int val3 = get_hash({i, k});
              
            freq[val1]++;
            freqEdges[val1].insert(val2);
            freqEdges[val2].insert(val1);
            
            freq[val2]++;
            freqEdges[val2].insert(val3);   
            freqEdges[val3].insert(val2);
              
            freq[val3]++;
            freqEdges[val1].insert(val3);  
            freqEdges[val3].insert(val1);
             
            conf++;   
          
          }
        }
      }  
    }

   int max_freq = 0;
   int best_hash; 
    
   if(conf == 0)
   best_edge_gl = {-1, -1};   
   else 
   { for(pair<int, int> p: freq)
     { 
       order.insert({p.second, p.first});  
       
       if(p.second > max_freq)
       {  best_hash = p.first;
          max_freq = p.second;
       }
     }
    
     best_edge_gl = get_edge(best_hash);
   }
    
}

void modifyedge(pair<int, int> edge)
{   
    int hash_val_edge = get_hash(edge);
    
    order.erase({freq[hash_val_edge], hash_val_edge});
    
    freq[hash_val_edge] = 0;
    
    if(weights.find(edge) != weights.end())
    { weights.erase(edge);
      
      int a = edge.first, b = edge.second;

      for(int idx=0; idx<adj_heur[a].size(); ++idx)
      { if(adj_heur[a][idx] == b)
        { adj_heur[a].erase(adj_heur[a].begin() + idx);
          break;
        }
      }
     
      for(int idx=0; idx<adj_heur[b].size(); ++idx)
      { if(adj_heur[b][idx] == a)
        { adj_heur[b].erase(adj_heur[b].begin() + idx);
          break;
        }
      }
    }
    else
    { weights[edge] = 1;
      int a = edge.first, b = edge.second;
      adj_heur[a].push_back(b);
      adj_heur[b].push_back(a);
    }
    
    for(int idx1: freqEdges[hash_val_edge])
    {
      int cur_freq = freq[idx1];
      order.erase({cur_freq, idx1});
      if(cur_freq > 1) 
      order.insert({cur_freq - 1, idx1});  
          
      freq[idx1]--;
      freqEdges[idx1].erase(hash_val_edge);
    }
    
    best_edge_gl = {-1, -1};
    
    if(!order.empty())
    {   pair<int, int> front = *(order.begin());
    
        if(front.first > 0)
        best_edge_gl = get_edge(front.second); 
    }   
    
}

vector<pair<int, int>> get_modified_edges(int local_n, vector<vector<int>> &adj_list)
{
    n_heur = local_n;
    adj_heur = adj_list;
    
    for(int i=1; i<=n_heur; ++i)
    {
       for(int node: adj_list[i])
       { 
           if(i < node)
           { weights[{i, node}] = 1;
             orig_weights[{i, node}] = 1;
           }
       }
    }

    // iter_bound = INT_MAX; 
    timecheck2();

    int prev_conf = INT_MAX;

    while(1)
    {   
        conf = 0;
        findbest();
        
        if(conf >= prev_conf)
        {
           vis_res.assign(n_heur+1, 0);

           for(int i=1; i<=n_heur; ++i)
           {
               if(!vis_res[i])
               {
                 vector<int> comp;
                 dfs_res(i, comp);
            
                 int sum_deg = 0;
                 int sz = comp.size();
               
                 for(int nd: comp)
                 sum_deg += adj_heur[nd].size();
            
                 if(sum_deg != (sz*(sz-1)))
                 { int rem = sum_deg/2; 
                   int add = (sz*(sz-1) - sum_deg)/2;

                   if(rem <= add)
                   { 
                     for(int j=0; j<comp.size(); ++j)
                     { 
                       for(int k=(j+1); k<comp.size(); ++k)
                       {
                         int node1 = comp[j];
                         int node2 = comp[k];
                         
                         if(node1 < node2 && weights.find({node1, node2}) != weights.end())
                         weights.erase({node1, node2});
                         else if(node1 > node2 && weights.find({node2, node1}) != weights.end())
                         weights.erase({node2, node1});
                       }
                     }
                    }
                    else
                    {
                     for(int j=0; j<comp.size(); ++j)  
                     { for(int k=(j+1); k<comp.size(); ++k)
                       { 
                         int node1 = comp[j];
                         int node2 = comp[k];
                         
                         if(node1 < node2 && weights.find({node1, node2}) == weights.end())
                         weights[{node1, node2}] = 1;
                         else if(node1 > node2 && weights.find({node2, node1}) == weights.end())
                         weights[{node2, node1}] = 1;
                       }
                     }
                    }

                   }
            
                 }
            }

           break;
        }
        else
        prev_conf = conf;
        
        if(best_edge_gl == make_pair(-1, -1))
        break;
        
        int iter = 0;

        while(iter < iter_bound)
        { if(best_edge_gl == make_pair(-1, -1))
          break;
          
          modifyedge(best_edge_gl);
          iter++;
        }

        freq.clear();
        freqEdges.clear();
        order.clear();
    }
    
    vector<pair<int, int>> modified_edges;
    
    int modif = 0;
    for(int i=1; i<=n_heur; ++i)
    { for(int j=(i+1); j<=n_heur; ++j) 
      { if((weights.find({i, j}) != weights.end()) != (orig_weights.find({i, j}) != orig_weights.end())) 
        {modif++;
         modified_edges.push_back({i, j});
        }      
      }
    }

    //cout<<"Modifications: "<<modif<<"\n";
    
    weights.clear();
    orig_weights.clear();
    freq.clear();
    freqEdges.clear();
    order.clear();
    
    return modified_edges;
}

pair<int,int> ap(int a,int b) 
{
    if(a>b) swap(a,b);
    return {a,b};
}

vector <int> com_no;
int cco=1;
int MS;
void setMS() {
    MS=350;
}
vector<pair<int,int>>reduce_component(int n,int m, vector<vector<int>> &adj_list,map<int,int>pin) {
    vector<pair<int,int>>mod;
    
    if(m <= 350000) 
    {
        mod=get_modified_edges(n, adj_list);
    }

    else {
        vector<bool>visit;
        visit.assign(n+1,false);
        
        for(int i=1;i<=n;i++) {
            if(!visit[i]) {
                
                int st=i;
                queue<pair<int,int>>q;
                
                vector<int>ll;
                ll.push_back(st);
                map<int,int>mb;
                mb[st]=1;
                q.push({st,0});
                visit[st]=true;

                while(!q.empty()) {
                    if(ll.size()>MS) {
                        break;
                    }
                    pair<int,int>x=q.front();
                    q.pop();
                        for(auto j:adj_list[x.first]) {
                            if(!visit[j]) {
                                mb[j]=1;
                                visit[j]=true;
                                q.push({j,x.second+1});
                                ll.push_back(j);
                                if(ll.size()>MS) {
                                    break;
                                }
                            }
                        }
                }
                int cn=ll.size();
                vector<vector<int>>cadj;
                map<int,int>in,out;
                cadj.resize(cn+1);
                int in1=1;
                for(auto i:ll) {
                    com_no[pin[i]]=cco;
                    out[i]=in1;
                    in[in1]=i;
                    in1++;
                }
                cco++;
                for(auto it:ll) {
                    for(auto jt:ll) {
                        if(it!=jt) {
                            if(org.find(ap(pin[it],pin[jt]))!=org.end()) {
                                cadj[out[it]].push_back(out[jt]);
                            }
                        }
                    }
                }

                vector<pair<int,int>>cmod=get_modified_edges(cn,cadj);
                for(auto j:cmod) {
                    mod.push_back(ap(in[j.first],in[j.second]));
                }
            }

        }
    }
    return mod;
}
vector<vector<int>> components2() {
    visited.assign(n+1,false);
    vector<vector<int>>co;
    for(int i=1;i<=n;i++) {
        if(visited[i]==false) {
            vector<int>ll;
        stack<int> s;
        s.push(i);
        int c=1;
        ll.push_back(i);
        while(!s.empty()) {
            int x=s.top();
            s.pop();
            visited[x]=true;
            for(auto it:adj[x]) {
                int a=x;
                int b=it;
                if(a>b) swap(a,b);
                if(!visited[it]) {
                    s.push(it);
                    ll.push_back(it);
                    visited[it]=true;
                    c++;
                }
            }
        } 
        co.push_back(ll);
        }
    }
    return co;
}


int main() 
{
    time(&start);
    
    string s1, s2; cin>>s1>>s2;
    cin>>n>>m;
    setMS();
    adj.resize(n+1);
    edge_set.resize(m);
    for(int i=0;i<m;i++) {
        int a,b;
        cin>>a>>b;
        if(a>b) swap(a,b);
        edge_set[i]={a,b};
        org[ap(a,b)]=1;
        adj[a].push_back(b);
        adj[b].push_back(a);
    }
    com_no.resize(n+1);
    com_no.assign(n+1,0);
    vector<vector<int>>co=components2();
    for(auto it:co) {
        if(it.size()>=3) {
            int n_n=it.size();
           
            map<int,int>out,in;
            int in1=1;
            for(auto i:it) {
                out[i]=in1;
                in[in1]=i;
                in1++;
            }

            vector<vector<int>>com_adj;
            com_adj.resize(n_n+1);
            int m_m=0;
            for(auto i:it) {
                for(auto j:adj[i]) {
                    com_adj[out[i]].push_back(out[j]);
                    m_m++;
                }
            }
            vector<pair<int,int>>mod=reduce_component(n_n,m_m,com_adj,in);
            for(auto i:mod) {
                mod_edges[ap(in[i.first],in[i.second])]=1;
            }


        }
    }
    for(auto i:edge_set) {
        if(com_no[i.first]!=com_no[i.second])
            mod_edges[ap(i.first,i.second)]=1;    
    }

    int modif = 0;
    vector<set<int>>madjr,madja;
    madja.resize(n+1);
    madjr.resize(n+1);
    
    for(auto i:mod_edges) {
        if(i.second==1) {
            modif++;
        }
    }

    vector<set<int>>madj;
    madj.resize(n+1);
    for(auto i:edge_set) {
      if(mod_edges.find(i)==mod_edges.end()) {
        madj[i.first].insert(i.second);
        madj[i.second].insert(i.first);
      }
    }
    for(auto i:mod_edges) {
      if(org.find(i.first)==org.end()) {
        madj[i.first.first].insert(i.first.second);
        madj[i.first.second].insert(i.first.first);
      }

    }
    vector<int>fcom(n+1);
    int fin=0;
    visited.assign(n+1,false);
    vector<set<int>>hc;
    for(int i=1;i<=n;i++) {
      if(!visited[i]) {
        stack<int>s;
        s.push(i);
        set<int>ss;
        ss.insert(i);
        fcom[i]=fin;
        visited[i]=true;
        while(!s.empty()) {
          int x=s.top();
          s.pop();
          for(auto j:madj[x]) {
            if(!visited[j]) {
              s.push(j);
              ss.insert(j);
              fcom[j]=fin;
              visited[j]=true;
            }
          }
        }
        hc.push_back(ss);
        fin++;
      }
    }

  
  bool once=false,done=false;
  int prev=-1;
    while(!tle) {
      int tcp=0;
      
      for(int i=1;i<=n&&!tle;i++) {
          
          int cur=fcom[i];
          int add=0;
          int bj=-1;
          int best=0;
          if(once) {
            best = -1;
          }
          map<int,int>cch;
          for(auto k:hc[cur]) {
            if(k!=i)
            if(org.find(ap(i,k))==org.end()) {
              add++;
            }
            else {
              add--;
            }
          }
          for(auto j:adj[i]) {
            if(fcom[i]!=fcom[j]) {
                if(cch.find(fcom[j])!=cch.end()) {
                    continue;
                }
                cch[fcom[j]]=1;
              int add1=add;
               int cur1=fcom[j];
               for(auto k:hc[cur1]) {
                 if(org.find(ap(i,k))==org.end()) {
                   add1--;
                 }
                 else {
                   add1++;
                 }
               }
               if(add1>best) {
                 bj=j;
                 best=add1;
               }
            }
          }
          
          int cb=0;
          if(bj!=-1) {
          
              for(auto k:hc[cur]) {
                if(k!=i) {
                    if(org.find(ap(i,k))==org.end()) {
                        mod_edges.erase(ap(i,k));
                        cb--;
                    }
                    else {
                        mod_edges[ap(i,k)]=1;
                        cb++;
                    }
                 
                }
              }
              for(auto k:hc[fcom[bj]]) {
               if(org.find(ap(i,k))==org.end()) {
                 mod_edges[ap(i,k)]=1;
                 cb++;
               }
               else {
                  mod_edges.erase(ap(i,k));
                  cb--;
               }
              }


              
              hc[cur].erase(i);
              fcom[i]=fcom[bj];
              hc[fcom[bj]].insert(i);
          }
          tcp+=cb;
          timecheck();
      }

      timecheck();
      if(once) {
        once=false;
      }

      if(tcp==0) {
        once=true;
      }
      prev=tcp;
      
    }

    int final_modif = 0;

    for(auto i:mod_edges) 
    {   
      final_modif++;
        cout<<i.first.first<<" "<<i.first.second<<endl;
    }
    // cout<<endl<<final_modif<<"\n";

    return 0;
}
