# Public Domain Pharmaceutical Information in Switzerland
### Introduction to the ywesee world: political and technical background

- Source of information: Zeno Davatz
- Edited by Alex Bettarini, Zeno Davatz
- 6.9.2020, 11.08


---
## Players

(in alphabetical order)

- **BAG**
	- BAG is only responsible for the pricing of medicines. The medical information side is done by Swissmedic, they "should" work together but they really don't.
	- they also have an Italian website
	- You can't get a price from BAG if you don't have a Swissmedic registration
	- If the product is on the BAG list, it's being paid by the health insurance of Switzerland

- **epha**
	- Separate company who used to release ATC-code based drug-drug *interactions* in the public domain, but they no longer do that, they are not making any updates at the moment

- **Galenica: Documed, e-mediat -> HCI-Solutions**
	- Galenica AG, headquartered in Bern, is an internationally active Swiss pharmaceutical and logistics company group. Galenica was split into two companies in 2014: Vifor Pharma and Galenica Santé. Also see: https://de.wikipedia.org/wiki/Galenica
	- e-mediat (now HCI-Solutions) copied the concept of the PZN (Pharmazentralnummer) and introduced the Pharmacode in Switzerland. The pharmacode is proprietary code and less exact then the Public Domain GTIN code that has the same function.
	- Documed and e-mediat where merged into HCI-Solutions. ywesee initiated two WEKO inquiries against Documed (2008) and HCI-Solutions (2014). Documed got fined CHF 50'000, HCI-Solutions/Galenica got fined 4.5 million CHF by WEKO. The second case is still being debated at the Bundesverwaltungsgericht (BVGER) of Switzerland because Galenica is not accepting the fine. Both WEKO inquiries have been big, they sent out questionnaires to all big pharmaceutical companies and stakeholders.
	- HCI-Solutions 
	makes these websites:
			- *Compendium* (proprietary, since 2000)
			- *AIPS* (government)
			- pharmavista.ch
			- SAI for refdata.ch (since 2020)

- **Refdata**
	- Refdata is a non-profit foundation, not a company. All the major stakeholders are members of that foundation
	- it's like a combination, it has all the medicines, and non-medical device which are being paid by the medical insurance
	- Refata gave the contract of building the SAI to HCI-Solutions (2020). The price of the SAI contract is not pubic.
	- The technical support of Refdata is done by HCI-Solutions

- **Swissmedic**
	- FDA is the equivalent in the USA. They supervise the approvals for pharmaceutical drugs.
	- A lot of the approvals that are done in Switzerland are also automatically approved for other countries in Africa that do not have such a thorough approval process.
	- they do not deal with prices

- **swisspeddose** 
	- swisspeddose.ch is public domain and distribute their data via swisspeddose.ch. This data is used in all AmiKo/CoMed databases. Swisspeddose is sponsored by the Swiss government with CHF 3.5 Million.
	- child dosage
	- made by an independent foundation

- **sappinfo**  
	- sappinfo.ch is public domain but not yet sponsored by the Swiss government. The goal is to work together with swisspeddose. 
	- The SAPP network collects information on medicines for *pregnant* and *breastfeeding* women.

- **ywesee**
	- competitor of HCI-Solutions

## Galenica vs ywesee
- **Wikipedia**
	- See: https://de.wikipedia.org/wiki/Galenica
- **Legal proceedings and outcomes**
	- Galenica bought Documend in 2004. Documed was founded in 1976. Documed has been publishing, since 1978, the drug compendium of Switzerland. Historically Swissmedic contracted Documed to publish their drug information, committing Documed to a monopoly that they defend with dubious means (according to the "Tages-Anzeiger" a daily newspaper published in Zurich). Documed filed their lawsuit against ywesee three days before christmas in 2003. After the BGE decision in 2008 Swissmedic had to "approve" the site of the open-source software company Ywesee. The Federal Administrative Court (Switzerland) then decided in June 2011 that Swissmedic's practice was neither appropriate nor necessary. As a result, Swissmedic was obliged to provide the drug information free of charge on an Internet platform called AIPS. The tender for the construction and operation of the platform was won by an offer from Documed, which according to the "Tages-Anzeiger" was noticeably below that of the competitors.
	- The first court case started in 2003. Documed sued ywesee (and was then bought by Galenica in 2004) because ywesee wrote a web-scraping spider agent that scraped all the information on the Documed website. ywesee won the case because the Fachinfo (specialist, professional) and Consumer (patient) info is required by law, therefore it cannot be copyright protected.
	- ywesee is no longer using that spider agent because after the second court case (BVGER 2011) Swissmedic had to do a public invitation to tender according to WTO rules (every government contract that exceeds CHF 275'000 has to be published in the official Schweizerisches Handelsamtsblatt SHAB) ywesee competed for the contract of AIPS, but ywesee was much more epensive than HCI-Solutions. HCI-Solutions offered to do the work for CHF 475,000 over 5 years, they have a hotline, support four languages; basically you can't do it for that amount of money, so the contract was subsidised by Galenica. That is how HCI-Solutoin won the public tender contract.<br>
		How does HCI-Solutions get their data?<br>
		Swissmedic, according with swiss law (AMZV) sends a out letters to the pharmaceutical companies which have Word files being uploaded into AIPS in each language, resulting in garbled XML data. They upload Word files into AIPS. AIPS has a web interface were you can publish information.
- **Dates**
	- 2004 ywesee won the **first** case against Documed, but in Switzerland you have to go through three courts. Documed also lost the second instance. Then the CEO of Documed said "We've made a mistake, we should never have sued Zeno" but then Galenica came and bought Documed and continued suing ywesee because they wanted to preserve their monopoly and they knew they would lose the monopoly if they didn't do it.
	- 2008 BGE case is closed
	- 2008 WEKO fines Documed with CHF 50'000
	- 2009 **second** case at the Bundesverwaltungsgericht (BVGER). The BVGER based their case on the BVG and WEKO case of 2008.
	- 2011 BVGER decision taken. Birth of the AIPS
	- 2017 WEKO II, the case being about HCI-Solutions and Galenica misusing their market power (not unlike the case of Microsoft accused of killing Netscape)
## Swissmedic
- **Historic developments that led to AIPS**
	- Swissmedic always said: "Fachinformationen (medical information leaflet) or Patienten-Information (leaflet in the package, for the consumer) have to be published in a complete database where all the information is in."
	- Because ywesee had all this info, Swissmedic allowed ywesee and the pharmaceutical companies to publish the consumer leaflet only online. At this stage the pharmaceutical companies were allowed to publish the consumer leaflet either on ODDB.org generika.cc. The Fachinfo still had to be published in paper form in a book, a very heavy book (dismissed on 1 Jan 2014). Then one pharmaceutical companies from the French part of Switzerland (Laboratoire Jacques Reboh) said we won't publish on ODDB.org or with HCI-Solutions, we will only publish our consumer leaflet on our website. Then Swissmedic said: "That's not okay" and they had a legal fight that went in front of the Bundesverwaltungsgericht (which takes care of all problem concerning the government). Then they decided that Swissmedic has to publish all the information on their website, in their own complete database with doctor and patient information in all languages. So Swissmedic said it's okay to publish the consumer leaflet at ywesee as well, because they are complete. Ywesee was much cheaper than Documed.
	- A lot of companies refused to publish their data on Compendium.ch website because it cost too much, e.g., a big pharma company paid CHF 1 million per year to publish their Fachinfo in the Compendium. The manager of Galenica was afraid that if Zeno wins the contract for AIPS everybody will publish there, and they lose control of the market. They won the contract by doing price dropping. The WEKO II case about them using their monopolistic power is still open. It is proven that they blackmailed companies: Documed through "HCI-Solutions" said <blockquote>if you don't pay for your price listing, GTIN, package size, galenic form, dosage, in our database we are going to drop your informtion, then if somebody wants to buy your product in the pharmacy, they won't find it</blockquote> They blackmailed the pharmaceutical companies.
	- The pharmaceutical companies don't care if they have to pay that amount or more per year. They just want their product to be sold at the highest price they can get, in the highly-priced country of Switzerland, so they would do anything to keep up that high price. Of course if they can save that CHF 1 million they will try to save it. Some people are afraid faster. They will do anything if they are told to do so. For example if a pharmacy in the Zurich hospital says: 
		<blockquote>we want to work with the data at HCI-Solutions...</blockquote>
so they started paying again to HCI-Solutions

## Compare three online databases

- AmiKo/ODDB <https://amiko.oddb.org/de/fi?gtin=7680517040107&type=atc&key=V10AX04>
- Swissmedic <http://www.swissmedicinfo.ch/ShowText.aspx?textType=FI&lang=de&authNr=51704> 
- Compendium <https://compendium.ch/mpro/mnr/25057/html/de?Platform=Desktop> is the Fachinfo website made by Documed.

## Files in `downloads/`

These files are downloaded in a step of the master build.sh file

- aips <http://download.swissmedicinfo.ch>
	- **aips.xml**
	- each line is a Fachinfo
	- The XML style is inconsistent; there are three types of XML code:
		1. 225 entries are multiline, starting with `<div id="monographie"><div class="MonTitle">`
			- they have lots of `<sub>`
		2. 9 entries  are a single long line, starting with `<div id="Monographie"><div class="Title">`
		3. 4070 entries are a single long line, starting with `<?xml version="1.0" encoding="utf-8"?>`

- bag <http://bag.e-mediat.net>
	- **bag_preparations.xml**

- Erweiterte_Arzneimittelliste HAM.xlsx

- epha
	- no longer used. It was used to get ATC codes from RN
	- **epha\_products\_de\_json.json**

- peddose <https://db.swisspeddose.ch>
	- **swisspeddosepublication-2019-02-21.xml**
	- section number and name
	- multiple ATC for one fachinfo
	- table looks
	- shorten some words

- refdata <http://refdatabase.refdata.ch>
	- **refdata\_pharma.xml**
	- no commas in the name
	- brand always uppercase
	- only products on the market

- swissmedic <https://www.swissmedic.ch>
	- **swissmedic_packages.xlsx**
	- the xlsx file format is a zip file (PK) containing a directory tree with XML files and other stuff
	- the brand name is not always uppercase
	- only one language
	- has more, they get ready to sell
	- animal health care also included
	- if column N is `A` and column W is `a` the categoty is `A+` which means "super strict" (for example narcotics)

## Files in `input/`

These files are under git

- **atc\_codes\_multi\_lingual.txt**

- ddd
	- <small>input/</small>
	- **atc\_ddd\_2019.csv**
	- for "pharma" utility
	- Reference files, to see the bad calculation
		- `downloads/20170313_ddd_oddb_Fehler.xlsx` (from customer criticizing `downloads/ddd.csv`, Fehler=mistake)
		- `downloads/ddd.csv` (created by Ruby)

- matrix
	- **matrix.csv**
	- <http://www.epha.org> European Public Health Alliance, a European non-profit association registered in Belgium

- sappinfo
	- <small>input/</small>
	- **sappinfo.xlxs**
	- two sheets

		- Breast-feeding (Stillzeit)
		- Pregnancy (Schwangerschaft)

## Files in `output/`

These files are created by the ywesee CLI utiities

- amiko\_db\_full\_idx\_de.db

	- a Python scripts uses it to creaate the frequencey DB file

- amiko\_report\_de.html

- pharma.csv
- pharma_report.html

## ywesee CLI utilities

The purpose is to create the input files required by the GUI applications. They used to be implemented as a single huge Java application. Now the functionality is split into various CLI commands implemented in C++

- cpp2sqlite
- interaction

	- Input: <http://download.epha.ch/data/matrix/matrix.csv>
	- Output DE: <http://pillbox.oddb.org/drug_interactions_csv_de.zip>
	- Output FR: <http://pillbox.oddb.org/drug_interactions_csv_fr.zip>

- pharma
	- it creates `pharma.csv` in directory `output/` then copied to `input/`
	- in `pharma.csv` <b>each line is one package info</b> (GTIN)
	- the file `pharma.csv` has only one language (German), but it contains also some French.

- sappinfo
- zurrose

## ywesee GUI apps

- AmiKo, for doctors
	- separate (GitHub) projects for each platform
		- [Windows](https://github.com/zdavatz/amiko_csharp)
			- language: C#
		- [iOS](https://github.com/zdavatz/AmiKo-iOS)
			- [User Manual](../amiko/doc-ios/UserManual.md) 
		- [macOS](https://github.com/zdavatz/amiko-osx)
		- [Android](https://github.com/zdavatz/AmiKo-Android)
		- [web](https://github.com/zdavatz/amiko-web)
			- Apache web server
			- NodeJS
		- [Linux](https://github.com/zdavatz/amiko_linux)
			- potentially multiplatform
			- language: C++
			- using wxWidgets

- Generika, for patients

## Glossary

- AIPS
	- Arzneimittel Informations Publikations System <https://de.wikipedia.org/wiki/Arzneimittelinformationspublikationssystem_AIPS>
- ATC
	- it should be 7 digits long, but it could be shorter, example `V10AX04` or broken down in the 5 levels `V 10 A X 04`
	- Anatomical Therapeutic Chemical Classification System <https://en.wikipedia.org/wiki/Anatomical_Therapeutic_Chemical_Classification_System>

		- The **first level** (first letter) of the code indicates the *anatomical* main group and consists of one letter. There are 14 main groups
		- The **second level** of the code indicates the *therapeutic* subgroup and consists of two digits
		- The **third level** of the code indicates the therapeutic/pharmacological subgroup and consists of one letter.
		- The **fourth level** of the code indicates the *chemical*/therapeutic/pharmacological subgroup and consists of one letter.
		- The **fifth level** of the code indicates the chemical substance and consists of two digits.

	- ATC/DDD Index 2019 <https://www.whocc.no/atc_ddd_index/>

	- If not in *aips* take it from *refdata*. If not in *refdata* take it from *swissmed*
	- ATC text: get French translation from `input/atc_codes_multi_lingual.txt`

- DDD: defined daily doses
- EAN: is the old name for GTIN 13
- "Galenic Form"

- GTIN:
	- it can be thought of as identifying a single medicine package
	- Global Trade Item Number <https://en.wikipedia.org/wiki/Global_Trade_Item_Number>

		<https://en.wikipedia.org/wiki/List_of_GS1_country_codes>
		
			760–769	 Switzerland and Liechtenstein
			800–839	 Italy, San Marino and Vatican City

	- checksum for GTIN <https://www.gs1.org/services/how-calculate-check-digit-manually>
	- it can be calculated from ATC
	- a barcode can represent the GTIN graphically
	- *regnr* is digits 5..9 of the GTIN
	- 13 digits:

				76      med
				80      Swiss
				12345   registration number
				123     package (column K of swissmedic)
				1       checksum

- ROA: route of administration
- WEKO: Wettbewerbs Kommission

---
# Links
- <https://uk.practicallaw.thomsonreuters.com/8-618-5625?transitionType=Default&contextData=(sc.Default)&firstPage=true&bhcp=1#co_anchor_a163148>
- <https://fr.wikipedia.org/wiki/Ywesee>
- <https://www.ywesee.com/Main/DocumedGalenicaVsYwesee>
- <https://www.ch.ch/en/demokratie/federalism/separation-of-powers/switzerlands-courts/>

	- Bundesgericht - Federal Supreme Court
	- Bundesstrafgericht - Federal Criminal Court
	- Bundesverwaltungsgericht - Federal Administrative Court
	- Bundespatentgericht - Federal Patent Court

- [Commercialisation of electronic drug information](https://www.ywesee.com/uploads/Main/Verfgung_WEKO_vom_19._Dezember_2016.pdf) Disposal of the WEKO
