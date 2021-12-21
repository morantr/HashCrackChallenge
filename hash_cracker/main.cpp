#include <time.h>
#include "base64.h"
#include "sha256.h"
#include <cstring>
#include <string.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

void _tmain(int argc, _TCHAR* argv[])
{
	SHA256 sha256;
	unsigned char HEX[32];
	const char  *cstr;
	string myHash;
	string  b64;
	string PassCpy;
	int CommonPasswordLine = 1;
	ofstream DecryptedPasswordFile;
	ifstream EncryptedPasswordsfile("EncryptedPasswords.txt");
	ifstream CommonPasswordsfile("passwords_db/10_million_password_list.txt");
	string EncryptedPass, CommonPassword;

	while (getline(CommonPasswordsfile, CommonPassword))
	{
		transform(CommonPassword.begin(), CommonPassword.end(), CommonPassword.begin(), ::tolower);
		PassCpy = CommonPassword;
		CommonPassword = "IEEE" + CommonPassword + "Xtreme";
		myHash = sha256(CommonPassword);
		cstr = myHash.c_str();
		for (int count = 0; count < sizeof(HEX) / sizeof(HEX[0]); count++) {
			sscanf_s(cstr, "%2hhx", &HEX[count]);
			cstr += 2;
		}
		b64 = base64_encode(HEX, 32);

		int EncryptedPassNum = 0;
		while (getline(EncryptedPasswordsfile, EncryptedPass))
		{
			if (b64 == EncryptedPass){
				cout << "Decrypted Password number: " << EncryptedPassNum << " - " << PassCpy <<  "\n";
				DecryptedPasswordFile.open("DecryptedPassword.txt", ios::app);
				DecryptedPasswordFile << EncryptedPassNum << ". " << PassCpy << "\n";
				DecryptedPasswordFile.close();
				break;
			}
			EncryptedPassNum++;

		}
		EncryptedPasswordsfile.clear();
		EncryptedPasswordsfile.seekg(0, ios::beg);
		CommonPasswordLine++;
	}
	
		//return 0;
}

