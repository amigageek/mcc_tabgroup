all: MCC_TabGroup.lha

.PHONY:MCC_TabGroup.lha
MCC_TabGroup.lha:
	lha ax=.vscode MCC_TabGroup.lha MCC_TabGroup MCC_TabGroup.info
