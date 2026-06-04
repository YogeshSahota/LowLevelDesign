#include <iostream>
#include <string>

using namespace std;

class ModelTrainer {
    public:
        void trainPipeline(const string &path){
            loadData(path);
            preprocessData();
            trainModel();
            evaluateModel();
            save();
        }
    
    protected:
        virtual void loadData(const string &path){
            cout << "Loading Data from " << path <<endl;
        }

        virtual void preprocessData(){
            cout << "Preprocessing data" <<endl;
        }

        virtual void trainModel() = 0;
        virtual void evaluateModel() = 0;

        virtual void save(){
            cout << "saving as default" <<endl;
        }
};

class NeuralNetworkTrainer: public ModelTrainer{
    protected:
        void trainModel() override{
            cout << "Neural Net training" << endl;
        }
        void evaluateModel() override{
            cout << "Neural Net evaluation" << endl;
        }

        void save() override{
            cout << "Neural Net custom save" << endl;
        }
};

class DecisionTreeTrainer: public ModelTrainer{
    protected:
        void trainModel() override{
            cout << "Decision Tree training" << endl;
        }
        void evaluateModel() override{
            cout << "Decision Tree evaluation" << endl;
        }
};

int main(){
    ModelTrainer* NWTrainer = new NeuralNetworkTrainer();
    ModelTrainer* DTTrainer = new DecisionTreeTrainer();

    NWTrainer->trainPipeline("NW folder");
    DTTrainer->trainPipeline("DT folder");
}