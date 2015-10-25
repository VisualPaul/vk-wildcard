VK cup 2015 wildcard round problem
==================================
Description of the problem
--------------------------
Given a set of source codes for some problem solutions find out, which participants cheated and copied other's solutions.

Quick approach
--------------
If source codes differs in size twice, than probably participants were fair. Otherwise let's look at two metrics: Levenstein distance of solutions, and numbers of common k-letter sequences. Both should be less than some constant.
(Of course it is possible to use some machine learning algorithm here, but I think that I can find 3 constants by hand). To find out number of equal k-letter sequences I use hashing.
To improve performance both in terms of computational resource usage and final performance of the algorith I turn source code into a sequence of new "characters": each character is either a special symbol, or some identifier found in the source code. Surprisingly replacement of identifiers with number of characters to the previous equal identifier *decreases* performance of an algorithm.