video:
	rm gource.txt
	gource --output-custom-log gource.txt
	(IFS="|"; while read wrong_name name; do sed -i "s/|$$wrong_name|/|$$name|/g" gource.txt; done < alias.txt)
	gource -f --path gource.txt --user-image-dir avatars -c 3
