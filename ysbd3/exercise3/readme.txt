Συνθέτες:
-->Γεώργιος Μπασιάκος - ΑΜ: 1115201700097
-->Ιωσήφ Χατζηπαυλής - ΑΜ: 1115201800299

Για την δημιουργία των εκτελέσιμων τρέξτε: 
make

Για να τρέξετε τα εκτελέσιμα (κατα προτίμηση):
./build/sort_main
./build/my_main 

Σύνδεσμος συνομιλίας:
https://chat.openai.com/share/abea2e9c-76ec-4c5a-a2a7-3e1db816e6b3

Γενικά σχόλια:
    Η εργασία φαίνεται να λειτουργεί άψογα. Χρησιμοποιήθηκε το μοντέλο γλωσσικής επεξεργασίας GPT-3.5, το οποίο ανταποκρίθηκε αρκετά 
ικανοποιητικά, φυσικά πάντα με την κατάλληλη καθοδήγηση. Το μεγαλύτερο πρόβλημα ήταν η αδυναμία του μοντέλου να συγκρατεί στη "μνήμη" 
του αρχεία που του είχαμε παρέχει προηγουμένως. Αυτό είχε ως αποτέλεσμα να του δίνουμε ανά τακτά χρονικά διαστήματα ξανά και ξανά, 
αρχεία (π.χ., τις βιβλιοθήκες μας), τα οποία του είχαμε δώσει πριν. Επίσης, καμιά φορά έκανε ανακύκλωση απαντήσεων, για τις οποίες 
είχαμε επισημάνει ότι δεν μας ικανοποιούν (π.χ., χρήση priority queue). Σε γενικές γραμμές, τα αρχεία chunk.c και sort.c υλοποιήθηκαν 
με μεγαλύτερη ευκολία από το μοντέλο, από το αρχείο merge.c. Για το αρχείο συγχώνευσης δυσκολευτήκαμε παρα πολύ να "συνεννοηθούμε" με 
το GPT. Σε κάποιο σημείο χρειάστηκε να του πούμε πώς θέλουμε να συνεχίσει την υλοποίηση. Οι αλλαγές στους κώδικες ωστόσο είναι ελάχιστες. 
Νομίζω ότι καταφέραμε επαρκώς, όλος σχεδόν ο κώδικας να παραχθεί από τον ψηφιακό βοηθό μας. Οι αλλαγές έχουν σχολιαστεί με "++" όταν 
πρόκειται για προσθήκη γραμμής (ή/και σπάνια αλλαγή) και με "++..." όταν αλλάχθηκαν παραπάνω απο 1 γραμμές (συνήθως 3 με 4 κοντινές
γράμμες πχ γραμμή: 55,56,57). Η χρήση της my_main() ειναι βοηθητική και αλλάχτηκαν απλά κάποιοι παράμετροι (πχ αριθμός εγγραφών). 
Η my_main() εμφανίζει τις εγγραφές πριν το σορτάρισμα και τις εγγραφές μετά το σορτάρισμα.
