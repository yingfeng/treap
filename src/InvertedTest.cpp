#include "InvertedIndex.cpp"
//#include "runlength.cpp"
int main (){
	//for (int j = 10000 ; j < 2000000;j+=10000) {
		int j = 0;
		float factor = 0.0;
		//for (float factor = 0.1;factor<3;factor+=0.2) {
			cout << "executing with factor = " << factor << endl;
			InvertedIndex *ii = new InvertedIndex("dataset/gov500.invlist","dataset/gov500.invlistfreq","dataset/gov500.words",500000,j,0);
			ii->buildTreap();
			size_t f_size = ii->estimateFreqSorted();
			vector<size_t> t = ii->estimateDocSorted();
			for (uint i = 0 ; i < t.size();i++) {
				cout << "Estimated doc_id " << t[i] << endl;
			}
			cout << "Estimated Frequency = " << f_size << endl;
			delete ii;
		//}
	//}
	//testRunLength();

}
