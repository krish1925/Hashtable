// NameTable.cpp

// This is a correct but inefficient implementation of
// the NameTable functionality.

#define BUCKET    100

#include "NameTable.h"
#include <string>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

  // This class does the real work of the implementation.

typedef struct {
    string key;
    int level;
    int value;
} HT_Item;

class Hash
{
  public:
    Hash(int size);
    ~Hash();
    void insertItem(HT_Item item);
    void deleteItem(string key);
    HT_Item* findItem(string key);
    int hashFunc(string key);
  private:
    vector<HT_Item> *table;
};

Hash::Hash(int size)
{
    table = new vector<HT_Item>[size];
}

Hash::~Hash()
{
    delete[] table;
}

int Hash::hashFunc(string key)
{
    int retval = 0;

    for (int i = 0; i < 2; i++)
        retval += (int) key[i];
    return retval % BUCKET;
}

void Hash::insertItem(HT_Item item)
{
    int index = hashFunc(item.key);
    table[index].push_back(item);
}

void Hash::deleteItem(string key)
{
    int index = hashFunc(key);
    table[index].pop_back();
}

HT_Item* Hash::findItem(string key)
{
    int index = hashFunc(key);
    int sz = table[index].size();
    static HT_Item item;
    while (sz > 0)
    {
        sz--;
        item = table[index][sz];
        if (item.key == key)
            return &item;
    }
    return NULL;
}

class NameTableImpl
{
  public:
    NameTableImpl();
    ~NameTableImpl();
    void enterScope();
    bool exitScope();
    bool declare(const string& id, int lineNum);
    int find(const string& id) const;
  private:
    vector<string> m_ids;
    vector<vector<string>> ids;
    int level;
    Hash *ht;
};

NameTableImpl::NameTableImpl()
{
    ht = new Hash(BUCKET);
    level = 0;
}

NameTableImpl::~NameTableImpl()
{
    delete ht;
}

void NameTableImpl::enterScope()
{
      // Extend the id vector with an empty string that
      // serves as a scope entry marker.

    m_ids.push_back("");
    level++;
}

bool NameTableImpl::exitScope()
{
      // Remove ids back to the last scope entry.

    while (!m_ids.empty()  &&  m_ids.back() != "")
    {
        ht->deleteItem(m_ids.back());
        m_ids.pop_back();
    }
    if (m_ids.empty())
        return false;

      // Remove the scope entry marker itself.

    m_ids.pop_back();
    level--;
    return true;
}

bool NameTableImpl::declare(const string& id, int lineNum)
{
    if (id.empty())
        return false;

      // Check for another declaration in the same scope.
      // Return if found, break out if encounter the scope
      // entry marker.

    HT_Item* itemp = ht->findItem(id);
    if (itemp && itemp->level == level) return false;

      // Save the declaration

    m_ids.push_back(id);

    HT_Item item;
    item.key = id;
    item.level = level;
    item.value = lineNum;
    ht->insertItem(item);
    return true;
}

int NameTableImpl::find(const string& id) const
{
    if (id.empty())
        return -1;

      // Search back for the most recent declaration still
      // available.

    HT_Item* item = ht->findItem(id);
    if (item) return item->value;
    return -1;
}

//*********** NameTable functions **************

// For the most part, these functions simply delegate to NameTableImpl's
// functions.

NameTable::NameTable()
{
    m_impl = new NameTableImpl;
}

NameTable::~NameTable()
{
    delete m_impl;
}

void NameTable::enterScope()
{
    m_impl->enterScope();
}

bool NameTable::exitScope()
{
    return m_impl->exitScope();
}

bool NameTable::declare(const string& id, int lineNum)
{
    return m_impl->declare(id, lineNum);
}

int NameTable::find(const string& id) const
{
    return m_impl->find(id);
}
