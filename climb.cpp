/*
	climb: aean's stats optimizer.
	2015.1.30
*/

#include "climb.h"


/** Settings. */
#define SIMC_NAME "simc64.exe"
const int amount = 3574 - 1844;
const char* stat_name[] = {
	"crit",
	"mastery",
	"haste",
	"multistrike",
	"versatility",
};
/* End of settings. */

const int stat_count = sizeof stat_name / sizeof *stat_name;
unsigned randtbl[stat_count][amount];

struct point_t{
	int stat[stat_count];
	point_t(){
		stat[stat_count - 1] = amount;
		for (int i = 0; i < stat_count - 1; i++){
			stat[i] = amount / stat_count;
			stat[stat_count - 1] -= stat[i];
		}
	}
	int go(int direct1, int direct2, int interval = 1){
		if (direct1 != direct2 && direct1 >= 0 && direct1 < stat_count && direct2 >= 0 && direct2 < stat_count){
			stat[direct1] += interval;
			stat[direct2] -= interval;
		} else return 0;
		for (int i = 0; i < stat_count; i++){
			if (stat[i] <= 0) return 0;
		}
		return 1;
	}
	unsigned zob(){
		unsigned z = 0;
		for (int i = 0; i < stat_count; i++){
			z ^= randtbl[i][stat[i]];
		}
		return z;
	}
};

struct simresult_t{
	double dps;
	double error;
	int iter;
	point_t point;
	simresult_t(point_t p, double d, double e, int i) :
		point(p), dps(d), error(e), iter(i) {}
};

const unsigned tt_size = 0x10000000;
unsigned tt[tt_size] = {0};
int transpose(unsigned zob){
	if (tt[zob & (tt_size - 1)] == zob)
		return 1;
	tt[zob & (tt_size - 1)] = zob;
	return 0;
}

void windows_sim(std::string tci, double& dps, double& error, int iterations){
	tci.insert(0, "\"" SIMC_NAME "\" output=out.txt ");
	tci.append("iterations=");
	char digits[16];
	sprintf(digits, "%d", iterations);
	tci.append(digits);
	tci.push_back(' ');
	PROCESS_INFORMATION ProcessInfo; 
    STARTUPINFO StartupInfo; //This is an [in] parameter 
    ZeroMemory(&StartupInfo, sizeof(StartupInfo)); 
    StartupInfo.cb = sizeof(StartupInfo); //Only compulsory field 
    if(CreateProcess(SIMC_NAME,(LPSTR)tci.c_str(), NULL,NULL,FALSE,0,NULL, NULL,&StartupInfo,&ProcessInfo)) { 
        WaitForSingleObject(ProcessInfo.hProcess,INFINITE); 
        CloseHandle(ProcessInfo.hThread); 
        CloseHandle(ProcessInfo.hProcess); 
    } else { 
        printf("The process could not be started...\n"); 
		abort();
    } 
    FILE* pfin=fopen( "out.txt", "r" ); 
    char* buffer; 
    buffer=(char*)malloc(sizeof(char)*4000); 
    fread(buffer,4000,1,pfin); 
    buffer=strstr(buffer,"DPS: "); 
    sscanf(buffer+5,"%lf",&dps); 
	buffer=strstr(buffer, "DPS-Error=");
    sscanf(buffer+10,"%lf",&error); 
	error *= 0.5;
    fclose(pfin); 
    remove("out.txt"); 
}

simresult_t sim(point_t& point){
	int iter = 8192;
	std::string tci(base_tci);
	char digits[16];
	for (int i = 0; i < stat_count; i++){		
		tci.append("gear_");
		tci.append(stat_name[i]);
		tci.append("_rating=");
		sprintf(digits, "%d", point.stat[i]);
		tci.append(digits);
		tci.push_back(' ');
	}
	double dps, error;
	windows_sim(tci, dps, error, iter);
	return simresult_t(point, dps, error, iter);
}

const simresult_t& sim(simresult_t& r, double target_error){
	int iter = r.iter;
	double error = r.error;
	double dps = r.dps;
	point_t point = r.point;
	if (error * 2.0 <= target_error) return r;
	std::string tci(base_tci);
	char digits[16];
	for (int i = 0; i < stat_count; i++){		
		tci.append("gear_");
		tci.append(stat_name[i]);
		tci.append("_rating=");
		sprintf(digits, "%d", point.stat[i]);
		tci.append(digits);
		tci.push_back(' ');
	}
	if (error * 2.0 > target_error){
		int more_iter = 1024 + static_cast<int>(iter * (error * 2.0 / target_error) * (error * 2.0 / target_error));
		more_iter -= iter;
		if (more_iter > 0) {
			if (more_iter > 100000) more_iter = 100000;
			double more_dps, more_error;
			windows_sim(tci, more_dps, more_error, more_iter);
			dps = (dps * iter) + (more_dps * more_iter);
			dps /= more_iter + iter;
			iter += more_iter;
			more_error = more_error * more_error * more_iter * more_iter;
			error = more_error + error * error * iter * iter;
			error /= more_iter + iter;
			error /= more_iter + iter;
			error = sqrt(error);
		}
	}
	r.iter = iter;
	r.dps = dps;
	r.error = error;
	return r;
};

#define lprintf(...) do{ printf(__VA_ARGS__); fprintf(log, __VA_ARGS__); fflush(log); } while(0)

void descent(){
	FILE* log = fopen("log.txt", "wb");
	for (int i = 0; i < stat_count; i++){
		for (int j = 0; j < amount; j++){
			randtbl[i][j] = (rand() << 24) ^ (rand() << 16) ^ (rand() << 8) ^ rand();
		}
	}
	point_t p;
	simresult_t res = sim(p);
	transpose(p.zob());
	int interval = 320;
	while (interval >= 40){
		lprintf("interval set to %d\r\n", interval);
		int found, reserved, confident;
		simresult_t reserved_res = res;
		do{
			found = 0;
			reserved = 0;
			confident = 1;
			for (int i = 0; i < stat_count; i++){
				for (int j = 0; j < stat_count; j++){
					point_t test(p);
					if (!test.go(i, j, interval))
						continue;
					if (transpose(test.zob()))
						continue;
					lprintf("test +%d%s -%d%s\r\n", interval, stat_name[i], interval, stat_name[j]);
					simresult_t testres = sim(test);
					lprintf("\tdps %.3lf %.3lf, sigma %.3lf %.3lf, delta %.3lf\r\n", testres.dps, res.dps, testres.error, res.error, abs(testres.dps - res.dps));
					while (1.47 * (testres.error + res.error) > abs(testres.dps - res.dps)){
						if (testres.iter + res.iter >= 800000) {
							if (!reserved && testres.dps >= res.dps){
								lprintf("\ttest iteration exceeded (%d), reserved\r\n", testres.iter);
								reserved = 1;
								reserved_res = testres;
								confident = 0;
								break;
							}
							else{
								lprintf("\ttest iteration exceeded (%d), but not reserved\r\n", testres.iter);
								confident = 0;
								break;
							}
						}
						if (res.error >= 0.8 * testres.error) sim(res, res.error * 1.8 * (abs(testres.dps - res.dps) / (1.47 * (testres.error + res.error))));
						else sim(testres, testres.error * 1.8 * (abs(testres.dps - res.dps) / (1.47 * (testres.error + res.error))));
						lprintf("\tdps %.3lf %.3lf, sigma %.3lf %.3lf, delta %.3lf\r\n", testres.dps, res.dps, testres.error, res.error, abs(testres.dps - res.dps));
					}
					if (confident && testres.dps > res.dps){
						res = testres;
						p = test;
						found = 1;
						lprintf("go +%s -%s, dps %.3lf, now at(", stat_name[i], stat_name[j], res.dps);
						for (int j = 0; j < stat_count; j++)
							lprintf("%d,", p.stat[j]);
						lprintf(")\r\n");
						break;
					}
					confident = 1;
				}
				if (found) break;
			}
			if (!found && reserved){
				p = reserved_res.point;
				res = reserved_res;
				lprintf("go reserved, dps %.3lf, now at(", res.dps);
				for (int j = 0; j < stat_count; j++)
					lprintf("%d,", p.stat[j]);
				lprintf(")\r\n");
				found = 1;
			}
		} while (found); 
		interval = static_cast<int>(interval * 0.618);
	}
}

void plot(){
	FILE* log = fopen("plot.txt", "wb");
	point_t p, q;
	p.stat[0] = 0;
	p.stat[1] = 0;
	p.stat[2] = amount;
	simresult_t res = sim(p);
	const int interval = 50;
	for (p.stat[0] = interval; p.stat[0] <= amount; p.stat[0] += interval){
		for (p.stat[1] = interval; p.stat[1] <= amount; p.stat[1] += interval){
			p.stat[2] = amount - p.stat[1] - p.stat[0];
			if (p.stat[2] > amount || p.stat[2] <= 0) continue;
			if (p.stat[0] < 550 || (p.stat[0] == 550 && p.stat[1] <= 900)) continue;
			res = sim(p);
			while (res.error * 2 > 5.0){
				res = sim(res, 5.0);
				if (res.iter > 400000) break;
			}
			lprintf("%d, %d, %.3lf, %.3lf\r\n", p.stat[0], p.stat[1], res.dps, res.error);
		}
	}
}

int main(){
	descent();
	// plot();
}