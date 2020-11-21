#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

class Apriori
{
    private:
    vector<vector<char> > Transactions;
    unordered_map<string, int> allItems;
    vector<string> allCandidateSets;
    vector<string> allFrequentSets;
    double minSupport;
    double minConfidence;

    public:
    Apriori(vector<vector<char> > Transactions, double Support, double minConfidence)
    {
        this->Transactions = Transactions;
        this->minSupport = (Support/100) * Transactions.size();
        this->minConfidence = minConfidence/100;
        aprioriAlgorithm();
    }

    bool candidatePruneHelper(string candidateItem, vector<string> frequentSet, int currIdx, string currStr)
    {
        if(currStr.size() == frequentSet[0].size())
        {
            for(int i = 0; i < frequentSet.size(); i++)
                if(frequentSet[i] == currStr)
                    return true;
            return false;
        }
        if(currIdx == candidateItem.size())
            return true;
        return candidatePruneHelper(candidateItem, frequentSet, currIdx + 1, currStr) && candidatePruneHelper(candidateItem, frequentSet, currIdx + 1, currStr + candidateItem[currIdx]);
    }

    void candidatePrune(vector<string>& candidateSet, vector<string> frequentSet)
    {
        for(int i = 0; i < candidateSet.size(); i++)
        {
            if(!candidatePruneHelper(candidateSet[i], frequentSet, 0, ""))
                candidateSet.erase(candidateSet.begin()+i), i--;
        }
    }

    vector<string> generateFirstCandidateSet()
    {
        vector<string> items;
        for(int i = 0; i < Transactions.size(); i++)
        {
            unordered_map<string,bool> repetitionCheck;
            for(int j = 0; j < Transactions[i].size(); j++)
            {
                string currItem(1,Transactions[i][j]);
                if(allItems[currItem] == 0)
                    items.push_back(currItem);
                if(!repetitionCheck[currItem])
                    allItems[currItem]++, repetitionCheck[currItem]=true;    
            }
        }
        return items;
    }

    void generateFrequentSet(vector<string>& items)
    {
        for(int i = 0; i < items.size(); i++)
            if(allItems[items[i]] < minSupport)
                items.erase(items.begin()+i) , i--;
    }

    void calculateSupport(vector<string> itemSet)
    {
        for(int z = 0; z < itemSet.size(); z++)
        {
            for(int i = 0; i < Transactions.size(); i++)
            {
                int x = 0, y = 0;
                while(x < Transactions[i].size())
                {
                    if(Transactions[i][x] == itemSet[z][y])
                        y++;
                    x++;
                }
                if(y == itemSet[0].size())
                    allItems[itemSet[z]]++;
            }
        }
    }

    vector<string> generateCandidateSet(vector<string> prevFreqItems)
    {
        int i = 0, j = 1;
        vector<string> newItemSet;
        while(i < prevFreqItems.size() - 1)
        {
            string temp1 = prevFreqItems[i].substr(0,prevFreqItems[i].size()-1);
            string temp2 = prevFreqItems[j].substr(0,prevFreqItems[j].size()-1);
            if(temp1 == temp2)
                newItemSet.push_back(prevFreqItems[i] + prevFreqItems[j][prevFreqItems[j].size()-1]);
            j++;
            if(j >= prevFreqItems.size())
                ++i, j=i+1;
        }
        return newItemSet;
    }

    vector<pair<string,string> > generateStrongRules(vector<string> itemSet)
    {
        vector<pair<string,string> > strongRules;
        for(int i = 0; i < itemSet.size(); i++)
        {
            string items = itemSet[i];
            string tempItems = itemSet[i];
            for(int j = 0; j < items.size(); j++)
            {
                bool check;
                for(int k = items.size()-1; k > 0 ; k--)
                {
                    string s1 = items.substr(0,k);
                    string s2 = items.substr(k,items.size()-k);
                    if(j > 0 && k < items.size()-1)
                    {
                        swap(s2[0],s2[1]);
                    }
                    double confidence = (double)allItems[tempItems]/(double)allItems[s1];
                    //cout << confidence << endl;
                    if(confidence >= minConfidence)
                    {
                        pair<string,string> temp(s1,s2); 
                        strongRules.push_back(temp);
                    }
                }
                if(check)
                    break;
                char temp = items[0];
                items.replace(0,items.size()-1,items.substr(1,items.size()-1));
                items[items.size()-1] = temp;
            }
        }
        return strongRules;
    }

    void aprioriAlgorithm()
    {
        vector<string> firstSet = generateFirstCandidateSet();
        generateFrequentSet(firstSet);
        cout << "Frequent set #1:" << endl << "====================" << endl;
        for(int i = 0; i < firstSet.size(); i++)
            cout << firstSet[i] << ' ' << allItems[firstSet[i]] << endl;
        cout << "----------------------------------------------------------------------" << endl;

        vector<string> currentFrequentSet = firstSet;
        vector<string> currentCandidateSet = generateCandidateSet(currentFrequentSet);
        calculateSupport(currentCandidateSet);
        generateFrequentSet(currentCandidateSet);
        cout << "Frequent set #2:" << endl << "====================" << endl;
        for(int i = 0; i < currentCandidateSet.size(); i++)
            cout << currentCandidateSet[i] << ' ' << allItems[currentCandidateSet[i]] << endl;

        vector<pair<string,string> >currentStrongRules = generateStrongRules(currentCandidateSet);
        cout << "Strong rules #2:" << endl << "====================" << endl;
        for(int i = 0; i < currentStrongRules.size(); i++)
            cout << currentStrongRules[i].first << " ---> " << currentStrongRules[i].second << endl;
        cout << "----------------------------------------------------------------------" << endl;

        int i = 3;
        while(true)
        {
            currentFrequentSet = currentCandidateSet;
            currentCandidateSet = generateCandidateSet(currentFrequentSet);
            if(currentCandidateSet.size() == 0)
                break;
            candidatePrune(currentCandidateSet, currentFrequentSet);        //Candidate Pruning
            calculateSupport(currentCandidateSet);                          //Calculating the support after pruning
            generateFrequentSet(currentCandidateSet);
            cout << "Frequent set #" << i << ':' << endl << "====================" << endl;
            for(int i = 0; i < currentCandidateSet.size(); i++)
                cout << currentCandidateSet[i] << ' ' << allItems[currentCandidateSet[i]] << endl;

            currentStrongRules = generateStrongRules(currentCandidateSet);
            cout << "Strong rules #" << i << ':' << endl << "====================" << endl;
            for(int i = 0; i < currentStrongRules.size(); i++)
                cout << currentStrongRules[i].first << " ---> " << currentStrongRules[i].second << endl;
            cout << "----------------------------------------------------------------------" << endl;
            i++;
        }
    }
};

int main()
{
   vector<vector<char> > Transactions = 
    {
        {'A', 'B', 'C', 'D', 'E', 'F'}, 
        {'A', 'C', 'D', 'E'}, 
        {'C', 'D', 'E'}, 
        {'B', 'F'}, 
        {'A', 'B', 'C', 'D'}, 
        {'A', 'B', 'E'}, 
        {'A', 'B', 'C'}, 
        {'A', 'B', 'E'}, 
        {'A', 'C'}, 
        {'A', 'C', 'D', 'E'}
    };
    double minSupport = 30;
    double minConfidence = 70;
    Apriori Test(Transactions, minSupport, minConfidence);
    return 0;
}
//{{'M', 'O', 'N', 'K', 'E', 'Y'}, {'D', 'O', 'N', 'K', 'E', 'Y'}, {'M', 'A', 'K', 'E'}, {'M', 'U', 'C', 'K', 'Y'}, {'C', 'O', 'O', 'K', 'I', 'E'}};