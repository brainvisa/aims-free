# This dictionary has been built from annex E of part 15 of the DICOM standard.
# Legend:
# - X means the attribute must be removed
# - U means the attribute must be replaced with a cleaned but internally consistent UUID
# - D means replace with a non-zero length dummy value
# - Z means replace with a zero or non-zero length dummy value
# - C means the attribute can be kept if it is cleaned
annex_e = {
    (0x0008,0x0050): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Accession Number
    (0x0018,0x4000): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Acquisition Comments
    (0x0040,0x0555): ['N', 'Y', 'X', '', '', '', '', '', '', '', 'C', ''], # Acquisition Context Sequence
    (0x0008,0x0022): ['N', 'Y', 'X/Z', '', '', '', '', 'K', 'C', '', '', ''], # Acquisition Date
    (0x0008,0x002A): ['N', 'Y', 'X/D', '', '', '', '', 'K', 'C', '', '', ''], # Acquisition DateTime
    (0x0018,0x1400): ['N', 'Y', 'X/D', '', '', '', '', '', '', 'C', '', ''], # Acquisition Device Processing Description
    (0x0018,0x9424): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Acquisition Protocol Description
    (0x0008,0x0032): ['N', 'Y', 'X/Z', '', '', '', '', 'K', 'C', '', '', ''], # Acquisition Time
    (0x0040,0x4035): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Actual Human Performers Sequence
    (0x0010,0x21B0): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Additional Patient's History
    (0x0038,0x0010): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Admission ID
    (0x0038,0x0020): ['N', 'N', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Admitting Date
    (0x0008,0x1084): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Admitting Diagnoses Code Sequence
    (0x0008,0x1080): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Admitting Diagnoses Description
    (0x0038,0x0021): ['N', 'N', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Admitting Time
    (0x0000,0x1000): ['N', 'N', 'X', '', 'K', '', '', '', '', '', '', ''], # Affected SOP Instance UID
    (0x0010,0x2110): ['N', 'N', 'X', '', '', '', 'C', '', '', 'C', '', ''], # Allergies
    (0x4000,0x0010): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Arbitrary
    (0x0040,0xA078): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Author Observer Sequence
    (0x0010,0x1081): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Branch of Service
    (0x0018,0x1007): ['N', 'Y', 'X', '', '', 'K', '', '', '', '', '', ''], # Cassette ID
    (0x0040,0x0280): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Comments on Performed Procedure Step
    (0x0020,0x9161): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Concatenation UID
    (0x0040,0x3001): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Confidentiality Constraint on Patient Data Description
    (0x0070,0x0084): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Content Creator's Name
    (0x0070,0x0086): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Content Creator's Identification Code Sequence
    (0x0008,0x0023): ['N', 'Y', 'Z/D', '', '', '', '', 'K', 'C', '', '', ''], # Content Date
    (0x0040,0xA730): ['N', 'Y', 'X', '', '', '', '', '', '', '', 'C', ''], # Content Sequence
    (0x0008,0x0033): ['N', 'Y', 'Z/D', '', '', '', '', 'K', 'C', '', '', ''], # Content Time
    (0x0008,0x010D): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Context Group Extension Creator UID
    (0x0018,0x0010): ['N', 'Y', 'Z/D', '', '', '', '', '', '', 'C', '', ''], # Contrast Bolus Agent
    (0x0018,0xA003): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Contribution Description
    (0x0010,0x2150): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Country of Residence
    (0x0008,0x9123): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Creator Version UID
    (0x0038,0x0300): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Current Patient Location
    #(0x5x,0xxxxx): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', 'C'], # Curve Data
    (0x0008,0x0025): ['Y', 'Y', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Curve Date
    (0x0008,0x0035): ['Y', 'Y', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Curve Time
    (0x0040,0xA07C): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Custodial Organization Sequence
    (0xFFFC,0xFFFC): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Data Set Trailing Padding
    (0x0008,0x2111): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Derivation Description
    (0x0018,0x700A): ['N', 'Y', 'X', '', '', 'K', '', '', '', '', '', ''], # Detector ID
    (0x0018,0x1000): ['N', 'Y', 'X/Z/D', '', '', 'K', '', '', '', '', '', ''], # Device Serial Number
    (0x0018,0x1002): ['N', 'Y', 'U', '', 'K', 'K', '', '', '', '', '', ''], # Device UID
    (0x0400,0x0100): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Digital Signature UID
    (0xFFFA,0xFFFA): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Digital Signatures Sequence
    (0x0020,0x9164): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Dimension Organization UID
    (0x0038,0x0040): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Discharge Diagnosis Description
    (0x4008,0x011A): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Distribution Address
    (0x4008,0x0119): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Distribution Name
    (0x300A,0x0013): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Dose Reference UID
    (0x0010,0x2160): ['N', 'Y', 'X', '', '', '', 'K', '', '', '', '', ''], # Ethnic Group
    (0x0008,0x0058): ['N', 'N', 'U', '', 'K', '', '', '', '', '', '', ''], # Failed SOP Instance UID List
    (0x0070,0x031A): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Fiducial UID
    (0x0040,0x2017): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Filler Order Number of Imaging Service Request
    (0x0020,0x9158): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Frame Comments
    (0x0020,0x0052): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Frame of Reference UID
    (0x0018,0x1008): ['N', 'Y', 'X', '', '', 'K', '', '', '', '', '', ''], # Gantry ID
    (0x0018,0x1005): ['N', 'Y', 'X', '', '', 'K', '', '', '', '', '', ''], # Generator ID
    (0x0070,0x0001): ['N', 'Y', 'D', '', '', '', '', '', '', '', '', 'C'], # Graphic Annotation Sequence
    (0x0040,0x4037): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Human Performers Name
    (0x0040,0x4036): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Human Performers Organization
    (0x0088,0x0200): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Icon Image Sequence
    (0x0008,0x4000): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Identifying Comments
    (0x0020,0x4000): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Image Comments
    (0x0028,0x4000): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Image Presentation Comments
    (0x0040,0x2400): ['N', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Imaging Service Request Comments
    (0x4008,0x0300): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Impressions
    (0x0008,0x0014): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Instance Creator UID
    (0x0008,0x0081): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Institution Address
    (0x0008,0x0082): ['N', 'Y', 'X/Z/D', '', '', '', '', '', '', '', '', ''], # Institution Code Sequence
    (0x0008,0x0080): ['N', 'Y', 'X/Z/D', '', '', '', '', '', '', '', '', ''], # Institution Name
    (0x0008,0x1040): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Institutional Department Name
    (0x0010,0x1050): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Insurance Plan Identification
    (0x0040,0x1011): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Intended Recipients of Results Identification Sequence
    (0x4008,0x0111): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Interpretation Approver Sequence
    (0x4008,0x010C): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Interpretation Author
    (0x4008,0x0115): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Interpretation Diagnosis Description
    (0x4008,0x0202): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Interpretation ID Issuer
    (0x4008,0x0102): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Interpretation Recorder
    (0x4008,0x010B): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Interpretation Text
    (0x4008,0x010A): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Interpretation Transcriber
    (0x0008,0x3010): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Irradiation Event UID
    (0x0038,0x0011): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Issuer of Admission ID
    (0x0010,0x0021): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Issuer of Patient ID
    (0x0038,0x0061): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Issuer of Service Episode ID
    (0x0028,0x1214): ['Y', 'N', 'U', '', 'K', '', '', '', '', '', '', ''], # Large Palette Color Lookup Table UID
    (0x0010,0x21D0): ['N', 'N', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Last Menstrual Date
    (0x0400,0x0404): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # MAC
    (0x0002,0x0003): ['N', 'N', 'U', '', 'K', '', '', '', '', '', '', ''], # Media Storage SOP Instance UID
    (0x0010,0x2000): ['N', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Medical Alerts
    (0x0010,0x1090): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Medical Record Locator
    (0x0010,0x1080): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Military Rank
    (0x0400,0x0550): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Modified Attributes Sequence
    (0x0020,0x3406): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Modified Image Description
    (0x0020,0x3401): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Modifying Device ID
    (0x0020,0x3404): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Modifying Device Manufacturer
    (0x0008,0x1060): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Name of Physician(0xs) Reading Study
    (0x0040,0x1010): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Names of Intended Recipient of Results
    (0x0010,0x2180): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Occupation
    (0x0008,0x1072): ['N', 'Y', 'X/D', '', '', '', '', '', '', '', '', ''], # Operators' Identification Sequence
    (0x0008,0x1070): ['N', 'Y', 'X/Z/D', '', '', '', '', '', '', '', '', ''], # Operators' Name
    (0x0400,0x0561): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Original Attributes Sequence
    (0x0040,0x2010): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Order Callback Phone Number
    (0x0040,0x2008): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Order Entered By
    (0x0040,0x2009): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Order Enterer Location
    (0x0010,0x1000): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Other Patient IDs
    (0x0010,0x1002): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Other Patient IDs Sequence
    (0x0010,0x1001): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Other Patient Names
    #(0x6x,0x4000): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', 'C'], # Overlay Comments
    #(0x6x,0x3000): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', 'C'], # Overlay Data
    (0x0008,0x0024): ['Y', 'Y', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Overlay Date
    (0x0008,0x0034): ['Y', 'Y', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Overlay Time
    (0x0028,0x1199): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Palette Color Lookup Table UID
    (0x0040,0xA07A): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Participant Sequence
    (0x0010,0x1040): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Patient Address
    (0x0010,0x4000): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Patient Comments
    (0x0010,0x0020): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Patient ID
    (0x0010,0x2203): ['N', 'Y', 'X/Z', '', '', '', 'K', '', '', '', '', ''], # Patient Sex Neutered
    (0x0038,0x0500): ['N', 'N', 'X', '', '', '', 'C', '', '', 'C', '', ''], # Patient State
    (0x0040,0x1004): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Patient Transport Arrangements
    (0x0010,0x1010): ['N', 'Y', 'X', '', '', '', 'K', '', '', '', '', ''], # Patient's Age
    (0x0010,0x0030): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Patient's Birth Date
    (0x0010,0x1005): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Patient's Birth Name
    (0x0010,0x0032): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Patient's Birth Time
    (0x0038,0x0400): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Patient's Institution Residence
    (0x0010,0x0050): ['', '', 'X', '', '', '', '', '', '', '', '', ''], # Patient's Insurance Plan Code Sequence
    (0x0010,0x1060): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Patient's Mother's Birth Name
    (0x0010,0x0010): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Patient's Name
    (0x0010,0x0101): ['', '', 'X', '', '', '', '', '', '', '', '', ''], # Patient's Primary Language Code Sequence
    (0x0010,0x0102): ['', '', 'X', '', '', '', '', '', '', '', '', ''], # Patient's Primary Language Modifier Code Sequence
    (0x0010,0x21F0): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Patient's Religious Preference
    (0x0010,0x0040): ['N', 'Y', 'Z', '', '', '', 'K', '', '', '', '', ''], # Patient's Sex
    (0x0010,0x1020): ['N', 'Y', 'X', '', '', '', 'K', '', '', '', '', ''], # Patient's Size
    (0x0010,0x2154): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Patient's Telephone Number
    (0x0010,0x1030): ['N', 'Y', 'X', '', '', '', 'K', '', '', '', '', ''], # Patient's Weight
    (0x0040,0x0243): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Performed Location
    (0x0040,0x0254): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Performed Procedure Step Description
    (0x0040,0x0253): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Performed Procedure Step ID
    (0x0040,0x0244): ['N', 'Y', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Performed Procedure Step Start Date
    (0x0040,0x0245): ['N', 'Y', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Performed Procedure Step Start Time
    (0x0040,0x0241): ['N', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Performed Station AE Title
    (0x0040,0x4030): ['N', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Performed Station Geographic Location Code Sequence
    (0x0040,0x0242): ['N', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Performed Station Name
    (0x0040,0x0248): ['N', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Performed Station Name Code Sequence
    (0x0008,0x1052): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Performing Physicians' Identification Sequence
    (0x0008,0x1050): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Performing Physicians' Name
    (0x0040,0x1102): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Person Address
    (0x0040,0x1101): ['N', 'Y', 'D', '', '', '', '', '', '', '', '', ''], # Person Identification Code Sequence
    (0x0040,0xA123): ['N', 'Y', 'D', '', '', '', '', '', '', '', '', ''], # Person Name
    (0x0040,0x1103): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Person Telephone Numbers
    (0x4008,0x0114): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Physician Approving Interpretation
    (0x0008,0x1062): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Physician Reading Study Identification Sequence
    (0x0008,0x1048): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Physician(0xs) of Record
    (0x0008,0x1049): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Physician(0xs) of Record Identification Sequence
    (0x0040,0x2016): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Placer Order Number of Imaging Service Request
    (0x0018,0x1004): ['N', 'Y', 'X', '', '', 'K', '', '', '', '', '', ''], # Plate ID
    (0x0040,0x0012): ['N', 'N', 'X', '', '', '', 'C', '', '', '', '', ''], # Pre-Medication
    (0x0010,0x21C0): ['N', 'N', 'X', '', '', '', 'K', '', '', '', '', ''], # Pregnancy Status
    (0x0018,0x1030): ['N', 'Y', 'X/D', '', '', '', '', '', '', 'C', '', ''], # Protocol Name
    (0x0040,0x2001): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Reason for Imaging Service Request
    (0x0032,0x1030): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Reason for Study
    (0x0400,0x0402): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Referenced Digital Signature Sequence
    (0x3006,0x0024): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Referenced Frame of Reference UID
    (0x0040,0x4023): ['N', 'N', 'U', '', 'K', '', '', '', '', '', '', ''], # Referenced General Purpose Scheduled Procedure Step Transaction UID
    (0x0008,0x1140): ['N', 'Y', 'X/Z/U*', '', 'K', '', '', '', '', '', '', ''], # Referenced Image Sequence
    (0x0038,0x1234): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Referenced Patient Alias Sequence
    (0x0008,0x1120): ['N', 'Y', 'X', '', 'X', '', '', '', '', '', '', ''], # Referenced Patient Sequence
    (0x0008,0x1111): ['N', 'Y', 'X/Z/D', '', 'K', '', '', '', '', '', '', ''], # Referenced Performed Procedure Step Sequence
    (0x0400,0x0403): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Referenced SOP Instance MAC Sequence
    (0x0008,0x1155): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Referenced SOP Instance UID
    (0x0004,0x1511): ['N', 'N', 'U', '', 'K', '', '', '', '', '', '', ''], # Referenced SOP Instance UID in File
    (0x0008,0x1110): ['N', 'Y', 'X/Z', '', 'K', '', '', '', '', '', '', ''], # Referenced Study Sequence
    (0x0008,0x0092): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Referring Physician's Address
    (0x0008,0x0096): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Referring Physician's Identification Sequence
    (0x0008,0x0090): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Referring Physician's Name
    (0x0008,0x0094): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Referring Physician's Telephone Numbers
    (0x0010,0x2152): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Region of Residence
    (0x3006,0x00C2): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Related Frame of Reference UID
    (0x0040,0x0275): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Request Attributes Sequence
    (0x0032,0x1070): ['N', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Requested Contrast Agent
    (0x0040,0x1400): ['N', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Requested Procedure Comments
    (0x0032,0x1060): ['N', 'Y', 'X/Z', '', '', '', '', '', '', 'C', '', ''], # Requested Procedure Description
    (0x0040,0x1001): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Requested Procedure ID
    (0x0040,0x1005): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Requested Procedure Location
    (0x0000,0x1001): ['N', 'N', 'U', '', 'K', '', '', '', '', '', '', ''], # Requested SOP Instance UID
    (0x0032,0x1032): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Requesting Physician
    (0x0032,0x1033): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Requesting Service
    (0x0010,0x2299): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Responsible Organization
    (0x0010,0x2297): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Responsible Person
    (0x4008,0x4000): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Results Comments
    (0x4008,0x0118): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Results Distribution List Sequence
    (0x4008,0x0042): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Results ID Issuer
    (0x300E,0x0008): ['N', 'Y', 'X/Z', '', '', '', '', '', '', '', '', ''], # Reviewer Name
    (0x0040,0x4034): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Scheduled Human Performers Sequence
    (0x0038,0x001E): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Scheduled Patient Institution Residence
    (0x0040,0x000B): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Scheduled Performing Physician Identification Sequence
    (0x0040,0x0006): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Scheduled Performing Physician Name
    (0x0040,0x0004): ['N', 'N', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Scheduled Procedure Step End Date
    (0x0040,0x0005): ['N', 'N', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Scheduled Procedure Step End Time
    (0x0040,0x0007): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Scheduled Procedure Step Description
    (0x0040,0x0011): ['N', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Scheduled Procedure Step Location
    (0x0040,0x0002): ['N', 'N', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Scheduled Procedure Step Start Date
    (0x0040,0x0003): ['N', 'N', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Scheduled Procedure Step Start Time
    (0x0040,0x0001): ['N', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Scheduled Station AE Title
    (0x0040,0x4027): ['N', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Scheduled Station Geographic Location Code Sequence
    (0x0040,0x0010): ['N', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Scheduled Station Name
    (0x0040,0x4025): ['N', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Scheduled Station Name Code Sequence
    (0x0032,0x1020): ['Y', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Scheduled Study Location
    (0x0032,0x1021): ['Y', 'N', 'X', '', '', 'K', '', '', '', '', '', ''], # Scheduled Study Location AE Title
    (0x0008,0x0021): ['N', 'Y', 'X/D', '', '', '', '', 'K', 'C', '', '', ''], # Series Date
    (0x0008,0x103E): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Series Description
    (0x0020,0x000E): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Series Instance UID
    (0x0008,0x0031): ['N', 'Y', 'X/D', '', '', '', '', 'K', 'C', '', '', ''], # Series Time
    (0x0038,0x0062): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Service Episode Description
    (0x0038,0x0060): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Service Episode ID
    (0x0010,0x21A0): ['N', 'N', 'X', '', '', '', 'K', '', '', '', '', ''], # Smoking Status
    (0x0008,0x0018): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # SOP Instance UID
    (0x0008,0x2112): ['N', 'Y', 'X/Z/U*', '', 'K', '', '', '', '', '', '', ''], # Source Image Sequence
    (0x0038,0x0050): ['N', 'N', 'X', '', '', '', 'C', '', '', '', '', ''], # Special Needs
    (0x0008,0x1010): ['N', 'Y', 'X/Z/D', '', '', 'K', '', '', '', '', '', ''], # Station Name
    (0x0088,0x0140): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Storage Media File-set UID
    (0x0032,0x4000): ['Y', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Study Comments
    (0x0008,0x0020): ['N', 'Y', 'Z', '', '', '', '', 'K', 'C', '', '', ''], # Study Date
    (0x0008,0x1030): ['N', 'Y', 'X', '', '', '', '', '', '', 'C', '', ''], # Study Description
    (0x0020,0x0010): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Study ID
    (0x0032,0x0012): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Study ID Issuer
    (0x0020,0x000D): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Study Instance UID
    (0x0008,0x0030): ['N', 'Y', 'Z', '', '', '', '', 'K', 'C', '', '', ''], # Study Time
    (0x0020,0x0200): ['N', 'Y', 'U', '', 'K', '', '', '', '', '', '', ''], # Synchronization Frame of Reference UID
    (0x0040,0xDB0D): ['Y', 'N', 'U', '', 'K', '', '', '', '', '', '', ''], # Template Extension Creator UID
    (0x0040,0xDB0C): ['Y', 'N', 'U', '', 'K', '', '', '', '', '', '', ''], # Template Extension Organization UID
    (0x4000,0x4000): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Text Comments
    (0x2030,0x0020): ['N', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Text String
    (0x0008,0x0201): ['N', 'Y', 'X', '', '', '', '', 'K', 'C', '', '', ''], # Timezone Offset From UTC
    (0x0088,0x0910): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Topic Author
    (0x0088,0x0912): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Topic Key Words
    (0x0088,0x0906): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Topic Subject
    (0x0088,0x0904): ['Y', 'N', 'X', '', '', '', '', '', '', '', '', ''], # Topic Title
    (0x0008,0x1195): ['N', 'N', 'U', '', 'K', '', '', '', '', '', '', ''], # Transaction UID
    (0x0040,0xA124): ['N', 'Y', 'U', '', '', '', '', '', '', '', '', ''], # UID
    (0x0040,0xA088): ['N', 'Y', 'Z', '', '', '', '', '', '', '', '', ''], # Verifying Observer Identification Code Sequence
    (0x0040,0xA075): ['N', 'Y', 'D', '', '', '', '', '', '', '', '', ''], # Verifying Observer Name
    (0x0040,0xA073): ['N', 'Y', 'D', '', '', '', '', '', '', '', '', ''], # Verifying Observer Sequence
    (0x0040,0xA027): ['N', 'Y', 'X', '', '', '', '', '', '', '', '', ''], # Verifying Organization
    (0x0038,0x4000): ['N', 'N', 'X', '', '', '', '', '', '', 'C', '', ''], # Visit Comments
    (0x0008,0x1150):['N', 'N', 'X', '', '', '', '', '', '', '', ''], # Referenced SOP Class UID Sequence
    (0x0018,0x1200):['N', 'N', 'X', '', '', '', '', '', '', '', ''], # Date of Last Calibration
    (0x0018,0x1201):['N', 'N', 'X', '', '', '', '', '', '', '', ''], # Time of Last Calibration
    (0x0032,0x1064):['N', 'N', 'X', '', '', '', '', '', '', '', ''], # Requested Procedure Sequence
    (0x0040,0x1012):['N', 'N', 'X', '', '', '', '', '', '', '', ''], # Reason for peformed procedure sequence
    (0x0040,0xA124):['N', 'N', 'X', '', '', '', '', '', '', '', ''], # UID
    (0x0072,0x6A):['N', 'N', 'X', '', '', '', '', '', '', '', ''], # Selector PN Value
    (0x3006,0xA6):['N', 'N', 'X', '', '', '', '', '', '', '', ''], # ROI Interpreter
}

# This dictionary contains tags that can be kept
safe_private_attributes = {
    "Philips PET Private Group": [(0x7053,0x0000),(0x7053,0x0009)],
    "Philips MR Imaging DD 001": [(0x2005,0x000d),(0x2005,0x000e)],
    "Philips Imaging DD 001": [(0x2001,0x0003),(0x2001,0x0004)],
    "Philips US Imaging DD 023": [(0x200d,0x0045)],
    "Philips US Imaging DD 033": [(0x200d,0x0000),(0x200d,0x0001),(0x200d,0x0002),(0x200d,0x0003),(0x200d,0x0004),(0x200d,0x0005),(0x200d,0x0006),(0x200d,0x0007),(0x200d,0x0008),(0x200d,0x000d),(0x200d,0x000f),(0x200d,0x0010),(0x200d,0x0011),(0x200d,0x0014),(0x200d,0x0021)],
    "Philips US Imaging DD 034": [(0x200d,0x0001),(0x200d,0x0002),(0x200d,0x0003),(0x200d,0x0004),(0x200d,0x0005),(0x200d,0x0008),(0x200d,0x0009),(0x200d,0x000a),(0x200d,0x000b),(0x200d,0x000c),(0x200d,0x000d),(0x200d,0x000e),(0x200d,0x000f),(0x200d,0x0010),(0x200d,0x0011),(0x200d,0x0012),(0x200d,0x0013),(0x200d,0x0014),(0x200d,0x0017),(0x200d,0x0018),(0x200d,0x001b),(0x200d,0x001c),(0x200d,0x001d),(0x200d,0x001e),(0x200d,0x001f),(0x200d,0x0020),(0x200d,0x0021),(0x200d,0x0022),(0x200d,0x0023),(0x200d,0x0024),(0x200d,0x0025),(0x200d,0x0026),(0x200d,0x0027),(0x200d,0x0028)],
    "Philips US Imaging DD 035": [(0x200d,0x0001),(0x200d,0x0003),(0x200d,0x0004),(0x200d,0x0007),(0x200d,0x0008),(0x200d,0x0009),(0x200d,0x000a),(0x200d,0x000c),(0x200d,0x000d)],
    "Philips US Imaging DD 036": [(0x200d,0x0015),(0x200d,0x0016),(0x200d,0x0017),(0x200d,0x0018),(0x200d,0x0019),(0x200d,0x0020)],
    "Philips US Imaging DD 038": [(0x200d,0x0001),(0x200d,0x0002),(0x200d,0x0003),(0x200d,0x0004)],
    "Philips US Imaging DD 039": [(0x200d,0x0001),(0x200d,0x0002),(0x200d,0x0003),(0x200d,0x0004),(0x200d,0x0005),(0x200d,0x0006),(0x200d,0x0007),(0x200d,0x0008),(0x200d,0x0009),(0x200d,0x000a),(0x200d,0x000b),(0x200d,0x000c),(0x200d,0x000d),(0x200d,0x0015)],
    "Philips US Imaging DD 040": [(0x200d,0x0001),(0x200d,0x0002),(0x200d,0x0003),(0x200d,0x0004),(0x200d,0x0005),(0x200d,0x0006),(0x200d,0x0007),(0x200d,0x0020)],
    "Philips US Imaging DD 042": [(0x200d,0x0015),(0x200d,0x0016),(0x200d,0x0020),(0x200d,0x0030),(0x200d,0x0031),(0x200d,0x0040),(0x200d,0x0050),(0x200d,0x0051),(0x200d,0x0052),(0x200d,0x0053),(0x200d,0x0054),(0x200d,0x0055),(0x200d,0x0056),(0x200d,0x0057),(0x200d,0x0058),(0x200d,0x0059),(0x200d,0x005a),(0x200d,0x005b),(0x200d,0x005c),(0x200d,0x005d),(0x200d,0x005e),(0x200d,0x005f),(0x200d,0x0060),(0x200d,0x0070),(0x200d,0x0071),(0x200d,0x0072),(0x200d,0x0073),(0x200d,0x0074),(0x200d,0x0075),(0x200d,0x0076),(0x200d,0x0077),(0x200d,0x0078),(0x200d,0x008c)],
    "Philips US Imaging DD 045": [(0x200d,0x00f1),(0x200d,0x00f3),(0x200d,0x00f4),(0x200d,0x00f5),(0x200d,0x00f6),(0x200d,0x00f8),(0x200d,0x00fa),(0x200d,0x00fb)],
    "Philips US Imaging DD 046": [(0x200d,0x0017)],
    "Philips US Imaging DD 048": [(0x200d,0x0001)],
    "Philips US Imaging DD 065": [(0x200d,0x0007)],
    "Philips US Imaging DD 066": [(0x200d,0x0000),(0x200d,0x0001),(0x200d,0x0003),(0x200d,0x0004)],
    "SIEMENS MR HEADER": [(0x0019,0x000c),(0x0019,0x000d),(0x0019,0x000e),(0x0019,0x0027)],
    "ELSCINT1": [(0x00E1,0x0021),(0x01E1,0x0026),(0x01E1,0x0050),(0x01F1,0x0001),(0x01F1,0x0007),(0x01F1,0x0026),(0x01F1,0x0027)],
    "GEMS_ACQU_01": [(0x0019,0x0023),(0x0019,0x0024),(0x0019,0x0027),(0x0019,0x009e)],
    "GEMS_SERS_01": [(0x0025,0x0007)],
    "GEMS_PARM_01": [(0x0043,0x0027),(0x0043,0x006f),(0x0043,0x0039)],
    "GEMS_HELIOS_01": [(0x0045,0x0001),(0x0045,0x0002)],
    "GEMS_SENO_02": [(0x0045,0x0006),(0x0045,0x001B),(0x0045,0x0020),(0x0045,0x0027),(0x0045,0x0029),(0x0045,0x002A),(0x0045,0x002B),(0x0045,0x0049),(0x0045,0x0058),(0x0045,0x0059),(0x0045,0x0060),(0x0045,0x0061),(0x0045,0x0064),(0x0045,0x0071),(0x0045,0x0072),(0x0045,0x0090),(0x0045,0x00A0),(0x0045,0x00A1),(0x0045,0x00A2),(0x0045,0x00A4),(0x0045,0x00A7),(0x0045,0x00A8)],
    "GEIIS PACS": [(0x0903,0x0010),(0x0903,0x0011),(0x0903,0x0012)],
    "HOLOGIC, Inc.": [(0x7e01,0x0001),(0x7e01,0x0002),(0x7e01,0x0010),(0x7e01,0x0011),(0x7e01,0x0012),(0x7f01,0x0001),(0x7f01,0x0002),(0x7f01,0x0010),(0x7f01,0x0011),(0x7f01,0x0012),(0x0019,0x0006),(0x0019,0x0007),(0x0019,0x0008),(0x0019,0x0016),(0x0019,0x0025),(0x0019,0x0026),(0x0019,0x0027),(0x0019,0x0028),(0x0019,0x0029),(0x0019,0x0030),(0x0019,0x0031),(0x0019,0x0032),(0x0019,0x0033),(0x0019,0x0034),(0x0019,0x0035),(0x0019,0x0037),(0x0019,0x0040),(0x0019,0x0041),(0x0019,0x0042),(0x0019,0x0043),(0x0019,0x0044),(0x0019,0x0045),(0x0019,0x0046),(0x0019,0x0050),(0x0019,0x0051),(0x0019,0x0052),(0x0019,0x0053),(0x0019,0x0060),(0x0019,0x0061),(0x0019,0x0062),(0x0019,0x0070),(0x0019,0x0071),(0x0019,0x0080),(0x0019,0x0085),(0x0019,0x0087),(0x0019,0x0089),(0x0019,0x008A),(0x0019,0x0090),(0x0019,0x0097),(0x0019,0x0098)],
    "LORAD Selenia": [(0x0019,0x0006),(0x0019,0x0007),(0x0019,0x0008),(0x0019,0x0016),(0x0019,0x0026),(0x0019,0x0027),(0x0019,0x0028),(0x0019,0x0029),(0x0019,0x0030),(0x0019,0x0031),(0x0019,0x0032),(0x0019,0x0033),(0x0019,0x0034),(0x0019,0x0035),(0x0019,0x0037),(0x0019,0x0040),(0x0019,0x0041),(0x0019,0x0050),(0x0019,0x0051),(0x0019,0x0052),(0x0019,0x0053),(0x0019,0x0060),(0x0019,0x0070),(0x0019,0x0071),(0x0019,0x0080),(0x0019,0x0090)],
    "NQHeader": [(0x0099,0x0001),(0x0099,0x0004),(0x0099,0x0005),(0x0099,0x0010),(0x0099,0x0020),(0x0099,0x0021)],
    "NQLeft": [(0x0199,0x0001),(0x0199,0x0002),(0x0199,0x0003),(0x0199,0x0004),(0x0199,0x0005),(0x0199,0x0006),(0x0199,0x0007),(0x0199,0x0008),(0x0199,0x0009),(0x0199,0x000a),(0x0199,0x000b),(0x0199,0x000c),(0x0199,0x000d),(0x0199,0x000e),(0x0199,0x000f),(0x0199,0x0010),(0x0199,0x0011),(0x0199,0x0012),(0x0199,0x0013),(0x0199,0x0014),(0x0199,0x0015),(0x0199,0x0016)],
    "NQRight": [(0x0299,0x0001),(0x0299,0x0002),(0x0299,0x0003),(0x0299,0x0004),(0x0299,0x0005),(0x0299,0x0006),(0x0299,0x0007),(0x0299,0x0008),(0x0299,0x0009),(0x0299,0x000a),(0x0299,0x000b),(0x0299,0x000c),(0x0299,0x000d),(0x0299,0x000e),(0x0299,0x000f),(0x0299,0x0010),(0x0299,0x0011),(0x0299,0x0012),(0x0299,0x0013),(0x0299,0x0014),(0x0299,0x0015),(0x0299,0x0016)]
}
