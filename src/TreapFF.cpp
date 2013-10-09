class StackHelp {
public:
	uint node;
	uint doc_id;
	uint freq;
	int r;
	StackHelp(uint node,uint doc_id,uint freq,int r){
		this->node = node;
		this->doc_id = doc_id;
		this->freq = freq;
		this->r = r;
	}
};

class TreapFF {
public:
	tree_ff *t;
	factorization *doc_ids;
	factorization *freqs;
	uint root;

	TreapFF(tree_ff *t) {
		this->t = t;
		this->root = this->t->Root_Node();
	}

	uint Left(uint x) {
		return this->t->First_Child(x);
	}
	uint Right(uint x) {
		return this->t->Next_Sibling(this->t->First_Child(x));
	}

	void Inorder(uint x,vector<pair<uint,uint> > &result) {
		if (x == (size_t)-1) return;
		uint y = Left(x);
		uint z = Right(x);
		Inorder(y);
		result.push_back(make_pair);
		Inorder(z);
	}
	uint GetDoc(uint x) {
		return doc_ids.get(t->Preorden_Rank(x)+1);
	} 
	uint GetFreq(uint x) {
		return freq.get(t->Preorden_Rank(x)+1);
	}

	// working on this...
	void Bfs(uint x,vector<pair<uint,uint> > &result) {
		Stack<StackHelp *> st; 
		result.push_back(make_pair(GetDoc(x),GetFreq(x)));
		StackHelp *sh_left = new StackHelp(Left(x),GetDoc(x),GetFreq(x),0)
		StackHelp *sh_right = new StackHelp(Right(x),GetDoc(x),GetFreq(x),1)
		st.push(sh_left);
		st.push(sh_right);
		while (!st.empty()) {
			StackHelp *sh = st.top();	
			uint old_id = st->doc_id;
			uint old_freq = st->freq;
			int b = st->b;
			uint y = st->node;
			if(b == 0) {
				uint doc_id = old_id - GetDoc(r);
			} else if (b == 0 { 
				uint doc_id = old_id + GetDoc(r);
			}			
			uint freq = old_f + GetFreq(r);
			result.push_back(make_pair<doc_id,freq>);
			st.pop();
			sh_left = new StackHelp(Left(y),doc_id,freq,0);
			sh_right = new StackHelp(Left(y),doc_id,freq,1);	
			st.push(sh_left);
			st.push(sh_right);
		}
	}
	static vector<uint> Intersect(tree_ff *t1, tree_ff *t2);
	static vector<uint> IntersectTopK(tree_ff *t1, tree_ff *t2, int k);
	static vector<uint> Union(tree_ff *t1, tree_ff *t2);
	static vector<uint> UnionTopK(tree_ff *t1, tree_ff *t2, int k);	
};