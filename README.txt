Progam Description
#Spam-Filter
This program uses a spam csv file to train a classifier to discriminate between spam and legit emails.

Input: Document D
fixed classes HAM and SPAM.
Training set of M hand labled documents (D1, C1), ..., (Dm, Cm)
Output: Naive Bayes classifier C = (SPAM || HAM)

Training.cc:
Input: spam.csv
sentence structure:
ham || spam,...,,,
... = DOC
Initial Probabilities:
P(SPAM) = SPAM(TRAIN_DOC) / TOTAL_DOCS 
P(HAM) = HAM(DOC) / TOTAL_DOCS
|V| =total number of words in SPAM and HAM

M = List of P(word,SPAM} and P(word, HAM) for all docs
For Each word i in DOC
  M[i][1] = P(word|SPAM) = (Count(word, SPAM) + 1) / ( TOTAL_COUNT(SPAM) + |V|)
  M[i][2] = P(word|HAM) = (Count(word, HAM) + 1) / ( TOTAL_COUNT(HAM) + |V|)
Output: M.txt

Testing.cc:
Input: M.txt, Text Document
Classifier:
 P(SPAM|TEST_DOC) = P(SPAM) * Product(P(all words In TEST_DOC corresponding to M|SPAM)
 P(HAM|TEST_DOC) = P(HAM) * Product(P(all words In TEST_DOC corresponding to M|HAM)
 Decision:
 If P(SPAM|TEST_DOC) > P(HAM|TEST_DOC)
  Output: "HAM"
 else
  Output: "SPAM"
