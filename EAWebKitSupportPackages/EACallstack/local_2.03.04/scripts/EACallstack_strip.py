import SimpleCodeStripper

codeStripper = SimpleCodeStripper.Strip(
		sourceSubdirectories = ['include', 'source', 'test', 'tool'],
		folderComponentsToExcludeFromCopy=['doc']
		)
