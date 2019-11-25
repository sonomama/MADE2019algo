#include <iostream>
#include <string>
#include <vector>


struct Node {
    Node(std::string key_): key(std::move(key_)),  \
                            deleted(false) {}
    std::string key;
    bool deleted;
};

class HashTable {
    public:
     HashTable(): buffer(default_size, nullptr),  \
                  buffer_size(default_size),  \
                  num_of_nodes(0) {}
     ~HashTable();
     HashTable(const HashTable &) = delete;
     HashTable(HashTable &&) = delete;
     HashTable &operator=(const HashTable &) = delete;
     HashTable &operator=(HashTable &&) = delete;
     
     bool has(const std::string key);
     bool add(const std::string key);
     bool remove(const std::string key);
     void print() const;

    private:
     const int default_size = 8;
     std::vector<Node*> buffer;
     size_t num_of_nodes;
     size_t buffer_size;

     void rehash();
    // Hashing is organized as h(k, i) = [h1(k) + i * h2(k)] % table_size,
    // so to have h(k, i) in range(0, table_size) we choose table_size as 2^n
    // and an odd h2(k) 
     int hash1(const std::string& str, size_t container_size) {
        // coeff1 must be coprime with table size
        const int coeff1 = 11; 
        int hash = 0;
        size_t size = str.size();
        for (int i = 0; i < size; ++i) 
        {
            hash = (hash * coeff1 + str[i]) % container_size;
        }
        return hash;
    }

     int hash2(const std::string& str, size_t container_size) {
        // coeff2 must be coprime with table size
        const int coeff2 = 131;
        int hash = 0;
        size_t size = str.size();
        for (int i = 0; i < size; ++i) 
        {
            hash = (hash * coeff2 + str[i]) % container_size;
        }
        return (2 * hash + 1) % container_size;
    }
};

HashTable::~HashTable(){
    for (int i = 0; i < buffer.size(); ++i) {
			delete buffer[i];
		}
}

void HashTable::rehash(){
    int new_buffer_size = 2 * buffer_size;
    std::vector<Node*> new_buffer(new_buffer_size, nullptr);
    for (int i=0; i < buffer_size; ++i) {
        if (buffer[i] && !buffer[i]->deleted) {
            // get new hash for the new table
            int h1 = hash1(buffer[i]->key, new_buffer_size);
            int h2 = hash2(buffer[i]->key, new_buffer_size);
            int count = 0;
            while(new_buffer[h1] && count < new_buffer_size) {
                h1 = (h1 + h2) % new_buffer_size;
                count++;
            }
            new_buffer[h1]= buffer[i];  
        } else {
            delete buffer[i];
        }
    }
    buffer = new_buffer;
    buffer_size = new_buffer_size;
};

bool HashTable::has(const std::string key){
    int h1 = hash1(key, buffer_size);
    int h2 = hash2(key, buffer_size);
    int count = 0;
    // initially all nodes are nullptr which is equivalent to their emptiness
    while (buffer[h1] && count < buffer_size) {
        if (buffer[h1]->key == key && !buffer[h1]->deleted) {
            return true;
        }
        h1 = (h1 + h2) % buffer_size;
        count++;
    }
    return false;
};

bool HashTable::add(const std::string key){
    double filled_ratio = 0.75; 
    if ((double)num_of_nodes/buffer_size >= filled_ratio) {
        rehash();
    }
    int h1 = hash1(key, buffer_size);
    int h2 = hash2(key, buffer_size);
    int count = 0;
    int deleted_key_ind = -1;
    // first we simultaneously check whether a key already exists
    // in the buffer and also look for the deleted keys
    while (buffer[h1] && count < buffer_size) {
        // if there is already such a key
        if (buffer[h1]->key == key && !buffer[h1]->deleted) {
            return false;
        }
        if (buffer[h1]->deleted && deleted_key_ind == -1) {
            deleted_key_ind = h1;
        }
        h1 = (h1 + h2) % buffer_size;
        count++;
    }
    // if we found a deleted node, we replace it
    if (deleted_key_ind != -1) {
        buffer[deleted_key_ind]->key = key;
        buffer[deleted_key_ind]->deleted = false;
    }
    else {
        buffer[h1] = new Node(key);
        
    }
    num_of_nodes++;
    return true;
};

bool HashTable::remove(const std::string key){
    int h1 = hash1(key, buffer_size);
    int h2 = hash2(key, buffer_size);
    int count = 0;
    while (buffer[h1] && count < buffer_size) {
        if (buffer[h1]->key == key && !buffer[h1]->deleted) {
            buffer[h1]->deleted = true;
            num_of_nodes--;
            return true;
        }
        h1 = (h1 + h2) % buffer_size;
        count++;
    }
    return false;
};

void HashTable::print() const {
    for(int i=0; i< buffer_size;++i) {
        if (buffer[i]) {
             std::cout << i << " " << buffer[i]->key << " ";
             std::cout << buffer[i]->deleted << std::endl;
        }
    }
}


int main() {
    HashTable table;
    char command = ' ';
	std::string key = "";
	while (std::cin >> command >> key) {
        switch (command) {
            case '?':
                std::cout << (table.has(key) ? "OK": "FAIL") << std::endl;
                break;
		    case '+':
                std::cout << (table.add(key) ? "OK": "FAIL") << std::endl;
                break;
		    case '-':
                std::cout << (table.remove(key) ? "OK": "FAIL") << std::endl;
                break;
		}
	}
}
