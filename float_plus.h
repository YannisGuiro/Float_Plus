#pragma once
#include<stdint.h>

class Float_plus
{
private:
	float precision = 0;
	uint32_t bits=0;
public:
	void Affiche_float(void);
	void Affiche_precision(void);
	void Affiche_binaire(void);
	void Affiche_mantisse(void);
	double get_double(void);
	uint32_t get_bits(void);
	Float_plus();
	Float_plus(unsigned int);
	Float_plus(float);
	Float_plus operator+(const Float_plus& fp);
	Float_plus operator-(const Float_plus& fp);
	Float_plus operator*(const Float_plus& fp);
	Float_plus operator/(const Float_plus& fp);
	Float_plus operator=(const float& fp);
	bool operator==(const unsigned int& fp) const;
	bool operator!=(const unsigned int& fp) const;
	bool operator==(const Float_plus& fp) const;
	bool operator!=(const Float_plus& fp) const;
	bool operator<=(const Float_plus& fp) const;
	bool operator<(const Float_plus& fp) const;
	bool operator>(const Float_plus& fp) const;
	bool operator>=(const Float_plus& fp) const;
};

Float_plus pow(Float_plus, unsigned int);

class Double_plus
{
private:
	double precision = 0;
	uint64_t bits=0;
public:
	void Affiche_float(void);
	void Affiche_precision(void);
	void Affiche_binaire(void);
	void Affiche_mantisse(void);
	double get_double(void);
	uint64_t get_bits(void);
	Double_plus();
	Double_plus(int);
	Double_plus(uint64_t);
	Double_plus(double);
	Double_plus operator+(const Double_plus& fp);
	Double_plus operator-(const Double_plus& fp);
	Double_plus operator*(const Double_plus& fp);
	Double_plus operator/(const Double_plus& fp);
	Double_plus operator=(const double& fp);
	bool operator==(const unsigned int& fp) const;
	bool operator==(const double& fp) const;
	bool operator==(const Double_plus& fp) const;
	bool operator!=(const unsigned int& fp) const;
	bool operator!=(const double& fp) const;
	bool operator!=(const Double_plus& fp) const;
	bool operator<=(const Double_plus& fp) const;
	bool operator<=(const double& fp) const;
	bool operator<(const Double_plus& fp) const;
	bool operator<(const double& fp) const;
	bool operator>(const Double_plus& fp) const;
	bool operator>(const double& fp) const;
	bool operator>=(const Double_plus& fp) const;
	bool operator>=(const double& fp) const;
};

Double_plus pow(Double_plus, unsigned int);

