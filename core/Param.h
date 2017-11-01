/*
 * Param.h
 *
 *  Created on: Jul 25, 2016
 *      Author: mason
 */

#ifndef PARAM_H_
#define PARAM_H_

#include "Eigen/Dense"
#include "BaseParam.h"

// Notice: aux is an auxiliary variable to help parameter updating
class Param : public BaseParam {
public:
#if USE_GPU
	  gpu_matrix aux_square;
	  gpu_matrix aux_mean;
#else
	  cpu_matrix aux_square;
	  cpu_matrix aux_mean;
#endif

    int iter;

    // allow sparse and dense parameters have different parameter initialization methods
    inline void initial(int outDim, int inDim) {
		val.init(outDim, inDim);
		grad.init(outDim, inDim);
        aux_square.init(outDim, inDim);
        aux_mean.init(outDim, inDim);

        dtype bound = sqrt(6.0 / (outDim + inDim + 1));
#if USE_GPU
		cval.init(outDim, inDim);
		cgrad.init(outDim, inDim);
		cval.random(bound);
		dump2gpu();
#else
        val.random(bound);
#endif
        iter = 0;
    }

    inline int outDim() {
        return val.row;
    }

    inline int inDim() {
        return val.col;
    }

    inline void clearGrad() {
        grad.zero();
    }

    void updateAdagrad(dtype alpha, dtype reg, dtype eps) {
        if (val.col > 1 && val.row > 1) {
            grad.special_add(grad, 1, val, reg);
        }
        aux_square.special_add1(aux_square, grad);
        val.special_add2(val, grad, aux_square, alpha, eps);
    }

    inline void randpoint(int& idx, int &idy) {
        //select indexes randomly
        std::vector<int> idRows, idCols;
        idRows.clear();
        idCols.clear();
        for (int i = 0; i < val.row; i++)
            idRows.push_back(i);
        for (int i = 0; i < val.col; i++)
            idCols.push_back(i);

        random_shuffle(idRows.begin(), idRows.end());
        random_shuffle(idCols.begin(), idCols.end());

        idy = idRows[0];
        idx = idCols[0];
    }

    inline dtype squareGradNorm() {
        dtype sumNorm = 0.0;
      /*  for (int i = 0; i < grad.size; i++) {
            sumNorm += grad.v[i] * grad.v[i];
        }*/
		sumNorm += grad.square_sum();
        return sumNorm;
    }

    inline void rescaleGrad(dtype scale) {
        /*grad.vec() = grad.vec() * scale;*/
		grad.multiply(grad, scale);
    }

    inline void save(std::ofstream &os)const {
        val.save(os);
        aux_square.save(os);
        aux_mean.save(os);
        os << iter << endl;
    }

    inline void load(std::ifstream &is) {
        val.load(is);
        aux_square.load(is);
        aux_mean.load(is);
        is >> iter;
    }
};

#endif /* PARAM_H_ */
