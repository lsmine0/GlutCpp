using namespace std;

#include <sstream>

template <typename TA>
class vec2 {
public:
	TA d[2];

	TA& operator[](int i) {
		return d[i];
	}

	TA& operator[](size_t i) {
		return d[i];
	}

	vec2& operator=(const TA& var) {
		d[0] = d[1] = var;
		return *this;
	}

	vec2& operator=(const TA(&arr)[2]) {
		d[0] = arr[0];
		d[1] = arr[1];
		return *this;
	}

	vec2& operator+=(const vec2& var) {
		d[0] += var.d[0];
		d[1] += var.d[1];
		return *this;
	}

	vec2& operator/=(const TA& var) {
		d[0] /= var;
		d[1] /= var;
		d[2] /= var;
		return *this;
	}

	vec2& operator-- (int) {
		d[0]--; d[1]--;
		return *this;
	}
};

template <typename TB>
class vec3 {
public:
	TB d[3];

	TB& operator[](int i) {
		return d[i];
	}

	TB& operator[](size_t i) {
		return d[i];
	}

	vec3& operator=(const TB& var) {
		d[0] = d[1] = d[2] = var;
		return *this;
	}

	vec3& operator=(const TB(&arr)[3]) {
		d[0] = arr[0];
		d[1] = arr[1];
		d[2] = arr[2];
		return *this;
	}

	vec3& operator+=(const vec3& var) {
		d[0] += var.d[0];
		d[1] += var.d[1];
		d[2] += var.d[2];
		return *this;
	}

	vec3& operator/=(const TB& var) {
		d[0] /= var;
		d[1] /= var;
		d[2] /= var;
		return *this;
	}

	vec3& operator-- (int) {
		d[0]--; d[1]--; d[2]--;
		return *this;
	}
};

template <typename TC>
class vec4 {
public:
	TC d[4];

	TC& operator[](int i) {
		return d[i];
	}

	TC& operator[](size_t i) {
		return d[i];
	}

	vec4& operator=(const TC& var) {
		d[0] = d[1] = d[2] = d[3] =var;
		return *this;
	}

	vec4& operator=(const TC(&arr)[4]) {
		d[0] = arr[0];
		d[1] = arr[1];
		d[2] = arr[2];
		d[3] = arr[3];
		return *this;
	}

	vec4& operator+=(const vec4& var) {
		d[0] += var.d[0];
		d[1] += var.d[1];
		d[2] += var.d[2];
		d[3] += var.d[3];
		return *this;
	}

	vec4& operator/=(const TC& var) {
		d[0] /= var;
		d[1] /= var;
		d[2] /= var;
		d[3] /= var;
		return *this;
	}

	vec4& operator-- (int) {
		d[0]--; d[1]--; d[2]--; d[3]--;
		return *this;
	}
};

template <typename TA>
istringstream& operator>>(istringstream& iss, vec2<TA>& obj) {
	iss >> obj.d[0] >> obj.d[1];
	return iss;
}

template <typename TB>
istringstream& operator>>(istringstream& iss, vec3<TB>& obj) {
	iss >> obj.d[0] >> obj.d[1] >> obj.d[2];
	return iss;
}

template <typename TC>
istringstream& operator>>(istringstream& iss, vec4<TC>& obj) {
	iss >> obj.d[0] >> obj.d[1] >> obj.d[2] >> obj.d[2];
	return iss;
}