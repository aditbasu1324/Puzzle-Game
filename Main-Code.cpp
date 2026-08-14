#include <iostream>
#include <vector>
#include <array>
#include <cmath>

using namespace std;
vector<int> vector_power(int exponent)
{
    // get 2**0+1, 2**1+1, 2**2+1,.. 2**N+1
    vector<int> result{2};
    int i{0};
    int base{2};
    while (i<exponent)
    {
        result.push_back(base+1);
        base = 2*base;
        i++;
    }
    return result;
}

// this function finds the bets to make 
void create_betting(int mid, int N, vector<long long>& bets, const vector<vector<double>>& all_probabilities, const vector<vector<long long>>& all_values, long long k_num, long long k_den, long long S)
{
    // the results of the code are printed out one branch at a time due to the nature of the code
    if (N==0) return;
    if (mid%2==1)
    {
        // bet to get value above above all_values[N][mid] i.e (all_values[N]-mid)/k rounded up
        long long bet{((all_values[N][mid+1]-S)*k_den-1)/k_num+1};
        cout << "Turn" << N << '\n';
        cout << "Current value: " << S/100 << '.'<< S%100 << '\n';
        cout << "Probability of success: " << all_probabilities[N][mid] << '\n';
        cout << "Bet is: " << bet/100 << '.' << bet%100 << '\n';
        bets.push_back(bet);
        // add k*bet 
        create_betting(mid/2+1, N-1, bets, all_probabilities,  all_values, k_num, k_den, llround(S+(k_num*bet)/k_den));
        create_betting(mid/2, N-1, bets, all_probabilities, all_values, k_num, k_den, S-bet);
    }
    else
    {
        long long bet{0};
        cout << "Turn" << N << '\n';
        cout << "Current value: " << S/100 << '.'<< S%100 << '\n';
        cout << "Probability of success: " << all_probabilities[N][mid] << '\n';
        cout << "Bet is: " << bet/100 << '.' << bet%100 << '\n';
        bets.push_back(bet);
        create_betting(mid/2, N-1, bets, all_probabilities, all_values,k_num, k_den, llround(S+(k_num*bet)/k_den));
        create_betting(mid/2, N-1, bets, all_probabilities, all_values,k_num, k_den, S-bet);
    }
}

// this approach is useful for multiple starting points of S since for each S you get an answer
void betting_game_grid(long long S, long long T, long long k_num,long long k_den,double p, int N)
{
    // note that the size of the interval grid is 2**N+1
    vector<int> powers {vector_power(N)};
    vector<long long> intervals(powers[N]);
    vector<long long> current_values {0, T};
    vector<double> current_probabilities {0,1};
    intervals[static_cast<size_t>(powers[N]-1)]=T;
    int i{0};
    // note that all probabilities will need to be stored when tracing back the bets or the loops
    vector<vector<double>> all_probabilities{};
    vector<vector<long long>> all_values{};
    all_probabilities.push_back(current_probabilities);
    all_values.push_back(current_values);
    while(i<N)
    {
        vector<long long> new_values{};
        vector<double> new_probabilities {};
        int current_size {static_cast<int>(current_values.size())};  
        for (int j{0}; j<current_size-1; j++)
        {
            new_values.push_back(current_values[j]);
            // this is the l+(u-l)/(k+1) step where second term is rounded up in order to stay in the range in case of loss
            // the -1, +1  ensure the rounding up occurs (as k_den+k_num>1)
            new_values.push_back(current_values[j]+((current_values[j+1]-current_values[j])*k_den-1)/(k_den+k_num)+1);
            new_probabilities.push_back(current_probabilities[j]);
            new_probabilities.push_back(current_probabilities[j]*(1-p)+current_probabilities[j+1]*p);
        }
        new_values.push_back(T);
        new_probabilities.push_back(1);
        i++;  
        current_probabilities = new_probabilities;
        current_values = new_values;
        all_probabilities.push_back(current_probabilities);
        all_values.push_back(current_values);
    }

    // binary search to find p, S based on T
    int low{0};
    int high{powers[N]-1};
    int mid{(low+high)/2};
    // Binary search, find the largest mid s.t curr_values[mid]<=S
    while(low<high)
    {
        mid = (low+high+1)/2;
        if (current_values[static_cast<size_t>(mid)]<=S)
        {
            low = mid;
        }
        else
        {
            high = mid-1;
        }
    }
    // loop ends when low = high
    mid = low;
    double probability{all_probabilities[N][static_cast<size_t>(mid)]};
    cout << "Probability of overall success: " <<  probability << '\n';
    // once you get the index and the probabilities, the bets can be backtracked
    // to find the bets use the index mid, need to know odd or even throughout
    vector<long long> bets{};
    // print out all probabilities and all_values for a clearer picture
    for (int i{N}; i>=0; i--)
    {
        for (int j{0}; j<powers[i]; j++)
        {
            cout << all_probabilities[i][j] << " ";
        }
        cout << '\n';
    }

    for (int i{N}; i>=0; i--)
    {
        for (int j{0}; j<powers[i]; j++)
        {
            cout << all_values[i][j] << " ";
        }
        cout << '\n';
    }
    create_betting(mid,N,bets,all_probabilities, all_values, k_num, k_den, S);
}

int main()
{
    double S{80};
    double T{250};
    double p{0.75};
    int N{3};
    long long S_cents = llround(S*100);
    long long T_cents = llround(T*100);
    long long k_num{5};
    long long k_den{6};
    betting_game_grid(S_cents,T_cents,k_num, k_den, p, N);
}

