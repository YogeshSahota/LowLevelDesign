#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

class FileSystemItem{
    public:
        virtual void ls(int intend = 0) = 0;
        virtual void openAll(int intend = 0) = 0;
        virtual int getSize() = 0;
        virtual string getName() = 0;
        virtual bool isFolder() = 0;
        virtual FileSystemItem* cd(const string &name) = 0;
};

class File: public FileSystemItem{
    private:
        string name;
        int size;
    
    public:
        File(string n, int s){
            // cout<<"File \n";
            name = n;
            size = s;
        }
        string getName() override{
            return name;
        }
        void ls(int intend = 0) override{
            cout << string(intend,' ') + getName() << endl;
        }
        void openAll(int intend = 0) override{
            cout << string(intend,' ') + getName() << endl;
        }
        int getSize() override{
            return size;
        }
        FileSystemItem* cd(const string &name) override {
            return nullptr;
        }
        bool isFolder() override{
            return false;
        }
};

class Folder:public FileSystemItem{
    private:
        string name;
        vector<FileSystemItem*> children;
    
    public:
        Folder(string n){
            // cout<<"Folder \n";
            name = n;
        }
        ~Folder() {
        for (auto c : children) delete c;
        }
        void add(FileSystemItem* child){
            children.push_back(child);
        }
        string getName() override{
            return name;
        }
        void ls(int intend = 0) override{
            for(auto child:children){
                if(child->isFolder()){
                    cout << string(intend,' ') + "+ "+child->getName() << endl;
                }else{
                    cout << string(intend,' ') + child->getName() << endl;
                }
            }
        }
        void openAll(int intend = 0) override{
            cout << string(intend,' ') + "+ "+ getName() << endl;
            for(auto child:children){
                child->openAll(intend+4);
            }
        }

        int getSize() override{
            int s = 0;
            for(auto child:children){
                s+=child->getSize();
            }
            return s;
        }
        FileSystemItem* cd(const string &name) override {
            for(auto child:children){
                if(child->isFolder() && child->getName()==name){
                    return child;
                }   
            }
            return nullptr;
        }
        bool isFolder() override{
            return true;
        }
};

int main(){
     // Build file system
    cout<<"Main \n";
    Folder* root = new Folder("root");
    root->add(new File("file1.txt", 1));
    root->add(new File("file2.txt", 1));

    Folder* docs = new Folder("docs");
    docs->add(new File("resume.pdf", 1));
    docs->add(new File("notes.txt", 1));
    root->add(docs);

    Folder* images = new Folder("images");
    images->add(new File("photo.jpg", 1));
    root->add(images);

    root->ls();

    docs->ls();

    root->openAll();

    FileSystemItem* cwd = root->cd("docs");
    if (cwd != nullptr) {
        cwd->ls();
    } else {
        cout << "\n Could not cd into docs \n";
    }

     cout << root->getSize();

    // Cleanup
    delete root;
    return 0;
}