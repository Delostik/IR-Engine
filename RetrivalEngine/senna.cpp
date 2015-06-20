extern "C" {

#include "senna.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "SENNA/SENNA_utils.h"
#include "SENNA/SENNA_Hash.h"
#include "SENNA/SENNA_Tokenizer.h"

#include "SENNA/SENNA_POS.h"
#include "SENNA/SENNA_NER.h"

	/* fgets max sizes */
#define MAX_SENTENCE_SIZE 1024

	char *opt_path = NULL;

	int *pos_labels = NULL;
	int *ner_labels = NULL;
	int init_flag = 0;

	SENNA_Hash *word_hash, *caps_hash, *suff_hash, *gazt_hash;
	SENNA_Hash *gazl_hash, *gazm_hash, *gazo_hash, *gazp_hash;
	SENNA_Hash *pos_hash, *ner_hash;
	SENNA_POS *pos;
	SENNA_NER *ner;
	SENNA_Tokenizer *tokenizer;

	void init() {
		init_flag = 1;
		/* inputs */
		word_hash = SENNA_Hash_new(opt_path, "hash/words.lst");
		caps_hash = SENNA_Hash_new(opt_path, "hash/caps.lst");
		suff_hash = SENNA_Hash_new(opt_path, "hash/suffix.lst");
		gazt_hash = SENNA_Hash_new(opt_path, "hash/gazetteer.lst");

		gazl_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.loc.lst", "data/ner.loc.dat");
		gazm_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.msc.lst", "data/ner.msc.dat");
		gazo_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.org.lst", "data/ner.org.dat");
		gazp_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.per.lst", "data/ner.per.dat");

		/* labels */
		pos_hash = SENNA_Hash_new(opt_path, "hash/pos.lst");
		ner_hash = SENNA_Hash_new(opt_path, "hash/ner.lst");
		pos = SENNA_POS_new(opt_path, "data/pos.dat");
		ner = SENNA_NER_new(opt_path, "data/ner.dat");

		tokenizer = SENNA_Tokenizer_new(word_hash, caps_hash, suff_hash, gazt_hash, gazl_hash, gazm_hash, gazo_hash, gazp_hash, 0);
	}

	int senna(const char* sentence) {
		if (!init_flag) { init(); }

		int i;
		FILE *fp;
		fp = fopen("pos_ner.dat", "w");

		SENNA_Tokens* tokens = SENNA_Tokenizer_tokenize(tokenizer, sentence);
		
		if (tokens->n > 0) {
			pos_labels = SENNA_POS_forward(pos, tokens->word_idx, tokens->caps_idx, tokens->suff_idx, tokens->n);
			ner_labels = SENNA_NER_forward(ner, tokens->word_idx, tokens->caps_idx, tokens->gazl_idx, tokens->gazm_idx, tokens->gazo_idx, tokens->gazp_idx, tokens->n);

			for (i = 0; i < tokens->n; i++) {
				fprintf(fp, "%15s", tokens->words[i]);
				fprintf(fp, "\t%10s", SENNA_Hash_key(pos_hash, pos_labels[i]));
				fprintf(fp, "\t%10s", SENNA_Hash_key(ner_hash, ner_labels[i]));
				fprintf(fp, "\n");
			}
			fprintf(fp, "\n");
		}
		fclose(fp);

		return 0;
	}

	void dispose() {
		SENNA_Tokenizer_free(tokenizer);

		SENNA_POS_free(pos);
		SENNA_NER_free(ner);

		SENNA_Hash_free(word_hash);
		SENNA_Hash_free(caps_hash);
		SENNA_Hash_free(suff_hash);
		SENNA_Hash_free(gazt_hash);
		SENNA_Hash_free(gazl_hash);
		SENNA_Hash_free(gazm_hash);
		SENNA_Hash_free(gazo_hash);
		SENNA_Hash_free(gazp_hash);

		SENNA_Hash_free(pos_hash);
		SENNA_Hash_free(ner_hash);
	}
}

