
typedef struct {
	float q[4];
	float beta;
	float zeta;
	float dt;
} Madgwick_t;

#ifdef __cplusplus
extern "C" {
#endif                                      
	void MadgwickInit(float _beta, float _zeta);
	Madgwick_t* getMadgwick();
	void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz);
#ifdef __cplusplus
}
#endif                                      
