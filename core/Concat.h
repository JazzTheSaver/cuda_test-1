#ifndef CONCAT
#define CONCAT

/*
 *  Concat.h:
 *  concatenatation operation.
 *
 *  Created on: Apr 22, 2017
 *      Author: mszhang
 */


#include "MyLib.h"
#include "Node.h"
#include "Graph.h"

class ConcatNode : public Node {
    public:
        vector<int> inDims;
        vector<PNode> ins;

        int nSize;

    public:
        ConcatNode() : Node() {
            node_type = "concat";
        }

        inline void clearValue() {
            Node::clearValue();
        }

    public:
        void forward(Graph *cg, const vector<PNode>& x) {
            if (x.size() == 0) {
                std::cout << "empty inputs for concat" << std::endl;
                return;
            }

            ins.clear();
            for (int i = 0; i < x.size(); i++) {
                ins.push_back(std::move(x[i]));
            }

            degree = 0;
            nSize = ins.size();
            for (int i = 0; i < nSize; ++i) {
                ins[i]->addParent(this);
            }

            cg->addNode(this);
        }


        void forward(Graph *cg, PNode x1, PNode x2) {
            ins.clear();
            ins.push_back(x1);
            ins.push_back(x2);

            degree = 0;
            for (int i = 0; i < 2; ++i) {
                ins[i]->addParent(this);
            }

            cg->addNode(this);
            nSize = 2;
        }

        void forward(Graph *cg, PNode x1, PNode x2, PNode x3) {
            ins.clear();
            ins.push_back(x1);
            ins.push_back(x2);
            ins.push_back(x3);

            degree = 0;
            for (int i = 0; i < 3; ++i) {
                ins[i]->addParent(this);
            }

            cg->addNode(this);
            nSize = 3;
        }

        void forward(Graph *cg, PNode x1, PNode x2, PNode x3, PNode x4) {
            ins.clear();
            ins.push_back(x1);
            ins.push_back(x2);
            ins.push_back(x3);
            ins.push_back(x4);

            degree = 0;
            for (int i = 0; i < 4; ++i) {
                ins[i]->addParent(this);
            }

            cg->addNode(this);
            nSize = 4;
        }

        void forward(Graph *cg, PNode x1, PNode x2, PNode x3, PNode x4, PNode x5) {
            ins.clear();
            ins.push_back(x1);
            ins.push_back(x2);
            ins.push_back(x3);
            ins.push_back(x4);
            ins.push_back(x5);

            degree = 0;
            for (int i = 0; i < 5; ++i) {
                ins[i]->addParent(this);
            }

            cg->addNode(this);

            nSize = 5;
        }

        void forward(Graph *cg, PNode x1, PNode x2, PNode x3, PNode x4, PNode x5, PNode x6) {
            ins.clear();
            ins.push_back(x1);
            ins.push_back(x2);
            ins.push_back(x3);
            ins.push_back(x4);
            ins.push_back(x5);
            ins.push_back(x6);

            degree = 0;
            for (int i = 0; i < 6; ++i) {
                ins[i]->addParent(this);
            }

            cg->addNode(this);

            nSize = 6;
        }



    public:
        inline PExecute generate(bool bTrain);

        // better to rewrite for deep understanding
        inline bool typeEqual(PNode other) {    //  ?? the same nSize
            return Node::typeEqual(other); //&& (nSize == ((ConcatNode*)other)->nSize);
        }

    public:
        inline void compute() {
            nSize = ins.size();
            inDims.clear();
            int curDim = 0;
            for (int i = 0; i < nSize; ++i) {
                inDims.push_back(ins[i]->val.size);
                curDim += inDims[i];
            }
            if (curDim != dim) {
                std::cout << "input dim size not match" << curDim << "\t" << dim << std::endl;
                return;
            }

            int offset = 0;
            for (int i = 0; i < nSize; ++i) {
                val.big_copy_small(offset, ins[i]->val);
                offset += inDims[i];
            }
        }

        void backward() {
            int offset = 0;
            cout <<"concat nsize:" << nSize << endl;
            for (int i = 0; i < nSize; ++i) {
                ins[i]->loss.short_add_long(ins[i]->loss, loss, offset);
                offset += inDims[i];
            }
        }
};


class ConcatExecute : public Execute {
    public:
        bool bTrain;
        int nSize;
    public:
        inline void  forward() {
            ofstream out("time", ios::app);
            auto start = std::chrono::high_resolution_clock::now();
            int count = batch.size();

            for (int idx = 0; idx < count; idx++) {
                ConcatNode* ptr = (ConcatNode*)batch[idx];
                ptr->compute();
                ptr->forward_drop(bTrain);
            }
            auto end = std::chrono::high_resolution_clock::now();
            out << "concat-forward " << std::chrono::duration<double>(end - start).count() << endl; 

        }

        inline void backward() {
            ofstream out("time", ios::app);
            auto start = std::chrono::high_resolution_clock::now();

            int count = batch.size();
            //#pragma omp parallel for schedule(static,1)
            for (int idx = 0; idx < count; idx++) {
                ConcatNode* ptr = (ConcatNode*)batch[idx];
                ptr->backward_drop();
                ptr->backward();
            }

            auto end = std::chrono::high_resolution_clock::now();
            out << "concat-backward " << std::chrono::duration<double>(end - start).count() << endl; 
        }
};

inline PExecute ConcatNode::generate(bool bTrain) {
    ConcatExecute* exec = new ConcatExecute();
    exec->batch.push_back(this);
    exec->bTrain = bTrain;
    exec->nSize = nSize;
    return exec;
}
//#endif

#endif
