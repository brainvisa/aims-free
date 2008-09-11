#ifndef AIMS_MATH_HYPERGEOMETRIC_H
#define AIMS_MATH_HYPERGEOMETRIC_H

double hyp0f1(double a, double *x, int n, int MAX);
double hyp1f1(double a, double b, double *x, int n, int MAX);
double	hypergeometric(double *p, int np, double *q, int nq,
	double *x, double *y, int n, double alpha, int MAX);


double loghyp0f1(double a, double *x, int n, int MAX);
double loghyp1f1(double a, double b, double *x, int n, int MAX);
double	loghypergeometric(double *p, int np, double *q, int nq,
	double *x, double *y, int n, double alpha, int MAX);

#endif
