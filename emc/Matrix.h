#pragma once
#include <initializer_list>
#include <vcruntime_string.h>

class Matrix
{
	int rows = 0;
	int cols = 0;
	double* pe = nullptr;

public:
	Matrix(int rows, int cols=1)
	{
		this->rows = rows;
		this->cols = cols;
		pe = new double[(long)rows * cols];
		memset(pe, 0, long(rows) * cols * sizeof(double));
	}

	Matrix(const Matrix& m)
	{
		rows = m.rows;
		cols = m.cols;
		pe = new double[(long)rows * cols];
		memcpy(pe, m.pe, (long)rows * cols * sizeof(double));
	}

	Matrix(const std::initializer_list<const std::initializer_list<double>> list);

	Matrix() = default;

	~Matrix()
	{
		delete[] pe;
	}

	Matrix& operator=(const Matrix& m)
	{
		if ((long)rows * cols != (long)m.rows * m.cols)
		{
			delete[] pe;
			pe = new double[(long)m.rows * m.cols];
		}
		rows = m.rows;
		cols = m.cols;
		memcpy(pe, m.pe, (long)rows * cols * sizeof(double));
		return *this;
	}

	void ResizeAndClear(int rows, int cols = 1)
	{
		delete[] pe;
		this->rows = rows;
		this->cols = cols;
		pe = new double[(long)rows * cols];
		memset(pe, 0, (long)rows * cols * sizeof(double));
	}

	void Clear()
	{
		memset(pe, 0, (long)rows * cols * sizeof(double));
	}

	int Rows() const
	{
		return rows;
	}

	int Cols() const
	{
		return cols;
	}

	double& operator()(int i, int j)
	{
		return pe[cols * i + j];
	}

	const double& operator()(int i, int j) const
	{
		return pe[cols * i + j];
	}

	double& operator()(int j)
	{
		return pe[j];
	}

	double operator()(int j) const
	{
		return pe[j];
	}

	double* data()
	{
		return pe;
	}

	const double* data() const
	{
		return pe;
	}

	static double Dot(const Matrix& m1, const Matrix& m2);

};

