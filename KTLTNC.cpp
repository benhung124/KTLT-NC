#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <map>
#include <windows.h>
#include <commdlg.h>
#include <algorithm>
#include <limits>
#include <cctype>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <regex>
#include <cmath>
#include <chrono>
#include <thread>
#include <atomic>
#include <random>
#include <random>
#include <chrono>
#include <set>
#include <unordered_set>

using namespace std;

void SetColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

static atomic<bool> examTimerRunning(false);
static atomic<int> examTimeRemaining(0);

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define YELLOW 6
#define WHITE 7

#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_MAGENTA 13
#define LIGHT_YELLOW 14
#define BRIGHT_WHITE 15

void startExamTimer(int seconds)
{
    examTimerRunning = true;
    examTimeRemaining = seconds;
    thread([]()
    {
        while (examTimerRunning && examTimeRemaining > 0)
        {
            int remaining = examTimeRemaining.load();
            int hours = remaining / 3600;
            int minutes = (remaining % 3600) / 60;
            int secs = remaining % 60;
            cout << "\rThoi gian con lai: " << setfill('0') << setw(2) << hours << ":"
                 << setw(2) << minutes << ":" << setw(2) << secs << " ";
            cout.flush();
            this_thread::sleep_for(chrono::seconds(1));
            examTimeRemaining--;
        }
        if (examTimerRunning && examTimeRemaining <= 0)
        {
            cout << "\rThoi gian con lai: 00:00:00                                  \n";
            cout.flush();
        }
    }).detach();
}

void stopExamTimer()
{
    examTimerRunning = false;
    cout << "\r" << string(60, ' ') << "\r";
    cout.flush();
}

// ========================== SAFE PARSING FUNCTIONS ==========================
int safeStoi(const string& s, int defaultVal = 0) {
    if (s.empty()) return defaultVal;
    try { return stoi(s); }
    catch (...) { return defaultVal; }
}

double safeStod(const string& s, double defaultVal = 0.0) {
    if (s.empty()) return defaultVal;
    try { return stod(s); }
    catch (...) { return defaultVal; }
}

string trimString(const string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool beginsWith(const string& s, const string& prefix)
{
    return s.rfind(prefix, 0) == 0;
}

string toUpperCopy(const string& s)
{
    string result = s;
    for (char& c : result)
        c = toupper((unsigned char)c);
    return result;
}

string extractFieldValue(const string& line, const string& prefix)
{
    if (!beginsWith(line, prefix))
        return "";
    return trimString(line.substr(prefix.size()));
}

bool parseYesNo(const string& s)
{
    string value = toUpperCopy(trimString(s));
    return value == "CO" || value == "YES" || value == "TRUE" || value == "1" || value == "OPEN";
}

int parseAnswerLetter(const string& s)
{
    string value = trimString(s);
    if (value.empty())
        return 0;
    char c = toupper((unsigned char)value[0]);
    if (c >= 'A' && c <= 'D')
        return c - 'A' + 1;
    return safeStoi(value);
}

int extractNumericSuffix(const string& s)
{
    string digits;
    for (char c : s)
    {
        if (isdigit((unsigned char)c))
            digits.push_back(c);
    }
    if (digits.empty())
        return 0;
    return safeStoi(digits);
}

vector<string> parseListSection(const vector<string>& lines, size_t startIndex)
{
    vector<string> values;
    for (size_t i = startIndex + 1; i < lines.size(); ++i)
    {
        string trimmed = trimString(lines[i]);
        if (trimmed.empty())
            break;
        if (trimmed.size() >= 1 && trimmed[0] == '-')
        {
            string item = trimString(trimmed.substr(1));
            if (!item.empty())
                values.push_back(item);
        }
        else
            break;
    }
    return values;
}

void VeKhungTieuDe(string tieuDe, int mau)
{
    int len = tieuDe.length() + 4;
    SetColor(mau);
    cout << "\n╔";
    for (int i = 0; i < len; i++)
        cout << "═";
    cout << "╗" << endl;
    cout << "║  ";
    SetColor(BRIGHT_WHITE);
    cout << tieuDe;
    SetColor(mau);
    cout << "  ║" << endl;
    cout << "╚";
    for (int i = 0; i < len; i++)
        cout << "═";
    cout << "╝" << endl;
    SetColor(WHITE);
}

string diemMonToString(const vector<pair<string, double>>& diemMon)
{
    stringstream ss;
    for (size_t i = 0; i < diemMon.size(); i++)
    {
        ss << diemMon[i].first << ":" << fixed << setprecision(2) << diemMon[i].second;
        if (i + 1 < diemMon.size()) ss << ",";
    }
    return ss.str();
}

vector<pair<string, double>> parseDiemMonString(const string& s)
{
    vector<pair<string, double>> result;
    if (s.empty()) return result;

    stringstream ss(s);
    string item;
    while (getline(ss, item, ','))
    {
        size_t pos = item.find(':');
        if (pos == string::npos) continue;
        string mon = item.substr(0, pos);
        string diemStr = item.substr(pos + 1);
        double diem = safeStod(diemStr, 0.0);
        if (!mon.empty()) result.emplace_back(mon, diem);
    }
    return result;
}

//======Tai file======
string taiFile()
{
    OPENFILENAMEW ofn;
    WCHAR szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);

    ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn))
    {
        char buffer[260];
        WideCharToMultiByte(CP_UTF8, 0, szFile, -1, buffer, sizeof(buffer), NULL, NULL);
        return string(buffer);
    }
    return "";
}

// ========================== CAU HOI ==========================
struct CauHoi
{
    int id;
    string noiDung;
    string dapAn[4];
    int dapAnDung;
};

// ========================== HAM TIEN ICH ==========================
// Shuffle câu hỏi
vector<CauHoi> shuffleCauHoi(const vector<CauHoi>& danhSach) {
    vector<CauHoi> result = danhSach;
    srand(time(0));
    unsigned seed = chrono::system_clock::now()
                    .time_since_epoch()
                    .count();

shuffle(result.begin(), result.end(),
        default_random_engine(seed));
    return result;
}

// Shuffle đáp án của một câu hỏi
void shuffleDapAn(CauHoi& ch) {
    int order[4] = {0, 1, 2, 3};
    srand(time(0));
unsigned seed = chrono::system_clock::now()
                    .time_since_epoch()
                    .count();

shuffle(order,
        order + 4,
        default_random_engine(seed));    
    string tempAns[4];
    for (int i = 0; i < 4; i++) {
        tempAns[i] = ch.dapAn[order[i]];
    }
    
    for (int i = 0; i < 4; i++) {
        ch.dapAn[i] = tempAns[i];
        if (tempAns[i] == ch.dapAn[ch.dapAnDung - 1]) {
            ch.dapAnDung = i + 1;
        }
    }
}

// Đếm ngược thời gian
void countdownTimer(int seconds) {
    while (seconds > 0) {
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        int secs = seconds % 60;
        
        cout << "\r";
        SetColor(LIGHT_RED);
        cout << "Thoi gian con lai: " << setfill('0') 
             << setw(2) << hours << ":" 
             << setw(2) << minutes << ":" 
             << setw(2) << secs << " ";
        SetColor(WHITE);
        cout.flush();
        
        Sleep(1000);
        seconds--;
    }
    SetColor(LIGHT_RED);
    cout << "\n*** HET GIO LAM BAI ***\n";
    SetColor(WHITE);
}

//===================== Ham Nhap ==========================
string inputLine(const string& prompt)
{
    string s;
    while (true)
    {
        cout << prompt;
        getline(cin, s);
        if (!s.empty())
            return s;
        cout << "Loi: khong duoc de trong.\n";
    }
}

template <typename T>
T nhapSo(const string& thongBao)
{
    T value;
    while (true)
    {
        cout << thongBao;
        if (cin >> value)
        {
            cin.ignore(1000, '\n');
            return value;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Nhap sai, thu lai!\n";
    }
}

time_t parseDateTime(const string& s)
{
    tm tmVal = {};
    int year, month, day, hour, minute;
    if (sscanf(s.c_str(), "%d/%d/%d %d:%d", &day, &month, &year, &hour, &minute) == 5)
    {
        tmVal.tm_year = year - 1900;
        tmVal.tm_mon = month - 1;
        tmVal.tm_mday = day;
        tmVal.tm_hour = hour;
        tmVal.tm_min = minute;
        tmVal.tm_sec = 0;
        tmVal.tm_isdst = -1;
        return mktime(&tmVal);
    }
    if (sscanf(s.c_str(), "%d-%d-%d %d:%d", &year, &month, &day, &hour, &minute) == 5)
    {
        tmVal.tm_year = year - 1900;
        tmVal.tm_mon = month - 1;
        tmVal.tm_mday = day;
        tmVal.tm_hour = hour;
        tmVal.tm_min = minute;
        tmVal.tm_sec = 0;
        tmVal.tm_isdst = -1;
        return mktime(&tmVal);
    }
    return 0;
}

string formatDateTime(time_t t)
{
    if (t <= 0)
        return "";
    tm localTime;
    localtime_s(&localTime, &t);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", &localTime);
    return string(buffer);
}

string hashPassword(const string& raw)
{
    return raw;
}

// ========================== DE THI ==========================
struct deThi
{
    int id;
    int khoa;
    string tenMon;
    string tenBaiThi;           // Tên bài thi
    vector<CauHoi> danhSachCauHoi;
    CauHoi ch;
    int thoiGianLamBai;
    int loaiThi;
    int phongThiId;
    time_t batDau;
    time_t ketThuc;
    bool locked;
    bool randomDe;              // Ngẫu nhiên câu hỏi
    bool shuffleAns;            // Xáo trộn đáp án
    vector<string> dsLopDuocThi;
    int soLanOnTap;

    deThi() : id(0), khoa(0), tenMon(""), tenBaiThi(""), thoiGianLamBai(0),
        loaiThi(1), phongThiId(0), batDau(0), ketThuc(0),
        locked(false), randomDe(false), shuffleAns(false), soLanOnTap(-1) {
    }

    void themCauHoi()
    {
        ch.id = nhapSo<int>("ID: ");
        cout << "Noi dung: ";
        getline(cin, ch.noiDung);
        for (int i = 0; i < 4; i++)
        {
            cout << "Lua chon " << i + 1 << ": ";
            getline(cin, ch.dapAn[i]);
        }
        do
        {
            ch.dapAnDung = nhapSo<int>("Dap an dung (1-4): ");
        } while (ch.dapAnDung < 1 || ch.dapAnDung > 4);
        danhSachCauHoi.push_back(ch);
    }

    void hienThiCauHoi()
    {
        for (auto& ch : danhSachCauHoi)
        {
            cout << ch.id << ". " << ch.noiDung << endl;
        }
    }

    bool isAvailable() const
    {
        if (locked)
            return false;
        if (batDau == 0 || ketThuc == 0)
            return true;
        time_t now = time(0);
        return now >= batDau && now <= ketThuc;
    }

    string getSchedule() const
    {
        string start = (batDau <= 0 ? string("Chua co") : formatDateTime(batDau));
        string end = (ketThuc <= 0 ? string("Chua co") : formatDateTime(ketThuc));
        return start + " -> " + end;
    }
};

// ========================== KET QUA ==========================
struct KetQua
{
    int idSV;
    string tenSV;
    string mon;
    double diem;
    int thoiGian;      // thời gian thực tế (s)
    int maDe;
    int loaiThi;
    time_t startTime;  // thời gian bắt đầu làm bài (server-side)
    time_t submitTime; // thời gian nộp bài
    string receiptId;  // id nộp bài (unique)
    string loginIP;    // IP đăng nhập
    int soLanLam;      // Số lần đã làm
    bool cameraOn;     // Camera thi chinh thuc
    string cameraNote; // Thong tin camera
    string getMon() const { return mon; }
};

// ========================== CLASS INFO ==========================
struct ClassInfo
{
    string id;
    string className;

    int coVanHocTapId;

    vector<int> danhSachGiangVien;
    vector<int> students;

    int studentCount;
    ClassInfo() : id(""), className(""), coVanHocTapId(0), studentCount(0) {}
};

struct PhongThi
{
    int id;
    string tenPhong;
    int sucChua;
    bool locked;

    PhongThi() : id(0), tenPhong(""), sucChua(0), locked(false) {}
};

// ========================== PERSON ==========================
class Person
{
protected:
    int id;
    string username;
    string password;
    string fullName;

public:
    Person() : id(0), username(""), password(""), fullName("") {}

    Person(int id, const string& username, const string& password, const string& fullName)
        : id(id), username(username), password(password), fullName(fullName)
    {
    }

    virtual ~Person() {}
    virtual string getRole() const = 0;



    virtual bool login(const string& u, const string& p) const
    {
        return u == username && (password == p || password == hashPassword(p));
    }

    int getId() const { return id; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getFullName() const { return fullName; }
    void setId(int newId) { id = newId; }
    void setUsername(const string& newUsername) { username = newUsername; }
    void setPassword(const string& newPassword) { password = newPassword; }
    void setFullName(const string& newFullName) { fullName = newFullName; }
    
};

// ========================== ADMIN ==========================
class Admin : public Person
{
public:
    Admin() : Person(1, "admin", "admin", "System Admin") {}

    Admin(int id, string u, string p, string ten) : Person(id, u, p, ten) {}

    string getRole() const override
    {
        return "Admin";
    }

    void hienThiThongTin() const
    {
        cout << "[ADMIN] " << fullName << endl;
    }

    string chuyenThanhChuoiFile() const
    {
        return to_string(id) + "|" + username + "|" + password + "|" + fullName;
    }
};

// ========================== QUAN LY THI ==========================
class QuanLyThi
{
private:
    string tenMon;
    deThi de;
    vector<KetQua> danhSachKetQua;
    vector<deThi> maDe;

public:
    void saveDeThi() const;
    void loadDeThi();
    vector<deThi>& getMaDe() { return maDe; }
    const vector<deThi>& getMaDe() const { return maDe; }
    QuanLyThi()
    {
        maDe = {};
        tenMon = "Chua dat ten";
    }

    void thietLapDeThi(int teacherId, string monGiangVien, const vector<ClassInfo>& classes, const vector<PhongThi>& examRooms)
    {
        (void)teacherId;
        (void)examRooms;
        do
        {
            de.loaiThi = nhapSo<int>("Loai bai thi (1. Chinh thuc | 2. On tap): ");
        } while (de.loaiThi != 1 && de.loaiThi != 2);

        de.tenMon = monGiangVien;
        
        // Thêm tên bài thi
        de.tenBaiThi = inputLine("Nhap ten bai thi: ");
        
        int nextId = 1;
        for (const auto& d : maDe)
            nextId = max(nextId, d.id + 1);
        de.id = nextId;
        SetColor(LIGHT_GREEN);
        cout << "Ma de tu dong duoc sinh: " << de.id << "\n";
        SetColor(WHITE);

        de.thoiGianLamBai = nhapSo<int>("Thoi gian lam bai (phut): ");
        
        // Tùy chọn random đề
        int chooseRandom = nhapSo<int>("Random cau hoi? (1. Co | 0. Khong): ");
        de.randomDe = (chooseRandom == 1);
        
        // Tùy chọn shuffle đáp án
        int chooseShuffle = nhapSo<int>("Xao tron dap an? (1. Co | 0. Khong): ");
        de.shuffleAns = (chooseShuffle == 1);

        de.phongThiId = 0;

        if (de.loaiThi == 2)
        {
            de.batDau = 0;
            de.ketThuc = 0;
            de.soLanOnTap = -1; // Default: unlimited practice attempts
        }
        else
        {
            de.soLanOnTap = 1; // Default: official exams can only be taken once
            do
            {
                string batDau = inputLine("Nhap ngay gio bat dau (dd/MM/yyyy HH:mm): ");
                de.batDau = parseDateTime(batDau);
                if (de.batDau == 0)
                    cout << "Dinh dang khong hop le. Thu lai!\n";
            } while (de.batDau == 0);

            do
            {
                string ketThuc = inputLine("Nhap ngay gio ket thuc (dd/MM/yyyy HH:mm): ");
                de.ketThuc = parseDateTime(ketThuc);
                if (de.ketThuc == 0 || de.ketThuc <= de.batDau)
                {
                    cout << "Khong hop le hoac ket thuc truoc bat dau. Thu lai!\n";
                    de.ketThuc = 0;
                }
            } while (de.ketThuc == 0);
        }

        de.dsLopDuocThi.clear();
        if (classes.empty())
        {
            SetColor(YELLOW);
            cout << "Chua co lop nao duoc dang ky. De thi se mo cho toan truong (ALL).\n";
            SetColor(WHITE);
        }
        else
        {
            cout << "\n===== DANH SACH LOP HIEN CO =====\n";
            for (const auto& c : classes)
            {
                cout << "ID " << c.id << " | " << c.className << "\n";
            }
            cout << "\nNhap ID cac lop duoc thi, cach nhau boi dau ',' (0 = tat ca lop). Ban co the nhap ID so hoac Ten lop: ";
            string line = inputLine("");
            stringstream ss(line);
            string token;
            vector<string> selectedTokens;
            bool allClasses = false;

            while (getline(ss, token, ','))
            {
                token = trimString(token);
                if (token.empty()) continue;
                if (token == "0")
                {
                    allClasses = true;
                    break;
                }
                selectedTokens.push_back(token);
            }

            if (allClasses)
            {
                SetColor(LIGHT_GREEN);
                cout << "De thi se mo cho tat ca lop.\n";
                SetColor(WHITE);
            }
            else
            {
                for (const auto& c : classes)
                {
                    for (const auto& sel : selectedTokens)
                    {
                        if (sel == c.id || sel == c.className)
                        {
                            de.dsLopDuocThi.push_back(c.className);
                            break;
                        }
                        int selNum = safeStoi(sel, INT_MIN);
                        if (selNum != INT_MIN && to_string(selNum) == c.id)
                        {
                            de.dsLopDuocThi.push_back(c.className);
                            break;
                        }
                    }
                }

                // Remove duplicates and normalize list
                sort(de.dsLopDuocThi.begin(), de.dsLopDuocThi.end());
                de.dsLopDuocThi.erase(unique(de.dsLopDuocThi.begin(), de.dsLopDuocThi.end()), de.dsLopDuocThi.end());

                if (de.dsLopDuocThi.empty())
                {
                    SetColor(YELLOW);
                    cout << "Khong co lop hop le duoc chon. De thi se mo cho toan truong (ALL).\n";
                    SetColor(WHITE);
                }
                else
                {
                    SetColor(GREEN);
                    cout << "De thi da duoc mo cho cac lop sau:\n";
                    for (const auto& lop : de.dsLopDuocThi)
                    {
                        cout << " - " << lop << "\n";
                    }
                    SetColor(WHITE);
                }
            }
        }

        de.locked = false;
        int m = nhapSo<int>("Nhap so cau hoi cua de: ");
        de.danhSachCauHoi.clear();
        for (int i = 0; i < m; i++)
        {
            SetColor(LIGHT_YELLOW);
            cout << "\n--- Nhap cau hoi thu " << i + 1 << " ---\n";
            SetColor(WHITE);
            de.themCauHoi();
        }

        maDe.push_back(de);
        saveDeThi();
        SetColor(LIGHT_GREEN);
        cout << "Tao de thi thanh cong!\n";
        SetColor(WHITE);
    }

    void khoaMoDeThi()
    {
        int idDe = nhapSo<int>("Nhap Ma De can thao tac: ");
        for (auto& d : maDe)
        {
            if (d.id == idDe)
            {
                cout << "1. Khoa de thi\n";
                cout << "2. Mo de thi va nhap lich thi\n";
                int action = nhapSo<int>("Chon: ");
                if (action == 1)
                {
                    d.locked = true;
                    cout << "De thi " << d.id << " da duoc khoa.\n";
                }
                else if (action == 2)
                {
                    string batDau;
                    string ketThuc;
                    time_t startTime = 0;
                    time_t endTime = 0;
                    do
                    {
                        batDau = inputLine("Nhap ngay gio bat dau (dd/MM/yyyy HH:mm): ");
                        startTime = parseDateTime(batDau);
                        if (startTime == 0)
                        {
                            cout << "Ngay gio khong hop le. Thu lai!\n";
                            continue;
                        }
                        ketThuc = inputLine("Nhap ngay gio ket thuc (dd/MM/yyyy HH:mm): ");
                        endTime = parseDateTime(ketThuc);
                        if (endTime == 0 || endTime <= startTime)
                        {
                            cout << "Khong hop le hoac ket thuc truoc bat dau. Thu lai!\n";
                            continue;
                        }
                        break;
                    } while (true);
                    d.batDau = startTime;
                    d.ketThuc = endTime;
                    d.locked = false;
                    cout << "De thi " << d.id << " da duoc mo va cap nhat lich thi.\n";
                }
                else
                {
                    cout << "Lua chon khong hop le.\n";
                    return;
                }
                saveDeThi();
                return;
            }
        }
        cout << "Khong tim thay de thi!\n";
    }

    void xoaDe()
    {
        string tenBaiThi = inputLine("Nhap ten bai thi can xoa: ");
        for (size_t i = 0; i < maDe.size(); i++)
        {
            if (maDe[i].tenBaiThi == tenBaiThi)
            {
                maDe.erase(maDe.begin() + i);
                saveDeThi();
                cout << "Xoa de thi '" << tenBaiThi << "' thanh cong!\n";
                return;
            }
        }
        cout << "Khong tim thay de thi voi ten nay!\n";
    }

    void xemLichThi()
    {
        if (maDe.empty())
        {
            cout << "Chua co de thi de hien thi lich.\n";
            return;
        }
        SetColor(CYAN);
        cout << "\n===== LICH THI =====\n";
        SetColor(WHITE);
        time_t now = time(0);
        for (const auto& d : maDe)
        {
            string status;
            if (d.locked)
            {
                status = "Khoa";
            }
            else if (d.batDau != 0 && now < d.batDau)
            {
                status = "Chua den gio";
            }
            else if (d.ketThuc != 0 && now > d.ketThuc)
            {
                status = "Da ket thuc";
            }
            else
            {
                status = "Dang mo";
            }

            string lopInfo;
            if (d.dsLopDuocThi.empty())
                lopInfo = "ALL";
            else
            {
                for (size_t i = 0; i < d.dsLopDuocThi.size(); i++)
                {
                    lopInfo += d.dsLopDuocThi[i];
                    if (i + 1 < d.dsLopDuocThi.size())
                        lopInfo += ", ";
                }
            }

            cout << "Ma de: " << d.id
                << " | Mon: " << d.tenMon;
            if (d.phongThiId != 0)
            {
                cout << " | Phong: " << d.phongThiId;
            }
            cout << " | Bat dau: " << (d.batDau == 0 ? string("Chua co") : formatDateTime(d.batDau))
                << " | Ket thuc: " << (d.ketThuc == 0 ? string("Chua co") : formatDateTime(d.ketThuc))
                << " | Lop: " << lopInfo
                << " | Trang thai: " << status
                << "\n";
        }
    }

    void clearRoomAssignments(int roomId)
    {
        for (auto& d : maDe)
        {
            if (d.phongThiId == roomId)
            {
                d.phongThiId = 0;
            }
        }
    }

    deThi* timDeThi(int id)
    {
        for (auto& d : maDe)
        {
            if (d.id == id)
                return &d;
        }
        return nullptr;
    }

    const deThi* timDeThi(int id) const
    {
        for (const auto& d : maDe)
        {
            if (d.id == id)
                return &d;
        }
        return nullptr;
    }

    void suaCauHoi()
    {
        int idDe = nhapSo<int>("Nhap Ma De can sua: ");
        for (auto& d : maDe)
        {
            if (d.id == idDe)
            {
                d.hienThiCauHoi();
                if (d.danhSachCauHoi.empty())
                {
                    SetColor(RED);
                    cout << "De chua co cau hoi!\n";
                    SetColor(WHITE);
                    return;
                }
                int idCH = nhapSo<int>("Nhap ID cau hoi can sua: ");
                for (auto& ch : d.danhSachCauHoi)
                {
                    if (ch.id == idCH)
                    {
                        cout << "Noi dung moi: ";
                        getline(cin, ch.noiDung);
                        for (int i = 0; i < 4; i++)
                        {
                            cout << "Lua chon " << i + 1 << ": ";
                            getline(cin, ch.dapAn[i]);
                        }
                        do
                        {
                            ch.dapAnDung = nhapSo<int>("Dap an dung (1-4): ");
                        } while (ch.dapAnDung < 1 || ch.dapAnDung > 4);

                        SetColor(LIGHT_GREEN);
                        cout << "Sua cau hoi thanh cong!\n";
                        SetColor(YELLOW);
                        return;
                    }
                }
                SetColor(LIGHT_RED);
                cout << "Khong tim thay cau hoi!\n";
                SetColor(RED);
                return;
            }
        }
        SetColor(LIGHT_RED);
        cout << "Khong tim thay de thi!\n";
        SetColor(WHITE);
    }

    void xoaCauHoi()
    {
        int idDe = nhapSo<int>("Nhap Ma De can xoa cau hoi: ");
        for (auto& d : maDe)
        {
            if (d.id == idDe)
            {
                d.hienThiCauHoi();
                int idCH = nhapSo<int>("Nhap ID cau hoi can xoa: ");
                auto it = remove_if(d.danhSachCauHoi.begin(), d.danhSachCauHoi.end(), [idCH](const CauHoi& c)
                    { return c.id == idCH; });
                if (it != d.danhSachCauHoi.end())
                {
                    d.danhSachCauHoi.erase(it, d.danhSachCauHoi.end());
                    SetColor(GREEN);
                    cout << "Xoa cau hoi thanh cong!\n";
                    SetColor(WHITE);
                }
                else
                {
                    SetColor(RED);
                    cout << "Khong tim thay cau hoi!\n";
                    SetColor(WHITE);
                }
                return;
            }
        }

        SetColor(RED);
        cout << "Khong tim thay de thi!\n";
        SetColor(WHITE);
    }

    void menu(int teacherId, string monGiangVien, const vector<ClassInfo>& classes, const vector<PhongThi>& examRooms)
    {
        int ch;
        do
        {
            VeKhungTieuDe("QUAN LY DE THI", YELLOW);
            cout << "1. Tao de thi moi\n2. Sua cau hoi trong de\n3. Xoa cau hoi trong de\n4. Xem danh sach de thi\n5. Xoa de thi\n6. Khoa/Mo de thi\n7. Xem lich thi\n0. Quay lai\n";
            ch = nhapSo<int>("Chon: ");
            switch (ch)
            {
            case 1:
                thietLapDeThi(teacherId, monGiangVien, classes, examRooms);
                break;
            case 2:
                suaCauHoi();
                break;
            case 3:
                xoaCauHoi();
                break;
            case 4:
                for (const auto& d : maDe)
                {
                    string loai = (d.loaiThi == 1) ? "[Chinh thuc]" : "[On tap]";
                    string randomStr = d.randomDe ? " | Tro cau: Co" : " | Tro cau: Khong";
                    string shuffleStr = d.shuffleAns ? " | Xao DA: Co" : " | Xao DA: Khong";
                    cout << "Ten bai thi: " << d.tenBaiThi
                        << " | Mon: " << d.tenMon
                        << " " << loai
                        << randomStr
                        << shuffleStr
                        << " | Trang thai: " << (d.locked ? "Khoa" : "Mo")
                        << endl;
                }
                break;
            case 5:
                xoaDe();
                break;
            case 6:
                khoaMoDeThi();
                break;
            case 7:
                xemLichThi();
                break;
            }
        } while (ch != 0);
    }
};

void QuanLyThi::saveDeThi() const
{
    ofstream fout("dethi.txt");
    for (const auto& de : maDe)
    {
        fout << de.id << '|' << de.tenMon << '|' << de.tenBaiThi << '|' << de.thoiGianLamBai << '|'
            << de.loaiThi << '|' << de.danhSachCauHoi.size()
            << '|' << de.phongThiId << '|' << (de.batDau > 0 ? formatDateTime(de.batDau) : string(""))
            << '|' << (de.ketThuc > 0 ? formatDateTime(de.ketThuc) : string("")) << '|' << (de.locked ? 1 : 0)
            << '|' << de.soLanOnTap << '|' << (de.randomDe ? 1 : 0)
            << '|' << (de.shuffleAns ? 1 : 0) << '|';

        for (size_t i = 0; i < de.dsLopDuocThi.size(); i++)
        {
            fout << de.dsLopDuocThi[i] << (i < de.dsLopDuocThi.size() - 1 ? "," : "");
        }
        if (de.dsLopDuocThi.empty())
            fout << "ALL";
        fout << '\n';

        for (const auto& ch : de.danhSachCauHoi)
        {
            fout << ch.id << '|' << ch.noiDung << '|' << ch.dapAn[0] << '|' << ch.dapAn[1] << '|' << ch.dapAn[2] << '|' << ch.dapAn[3] << '|' << ch.dapAnDung << '\n';
        }
    }
    fout.close();
}

void QuanLyThi::loadDeThi()
{
    maDe.clear();
    ifstream fin("dethi.txt");
    if (!fin) return;

    vector<string> lines;
    string line;
    while (getline(fin, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        lines.push_back(line);
    }
    fin.close();

    if (lines.empty())
        return;

    bool isMachineFormat = false;
    for (const auto& l : lines)
    {
        string trimmed = trimString(l);
        if (trimmed.empty())
            continue;
        if (trimmed.find('|') != string::npos)
            isMachineFormat = true;
        break;
    }

    if (isMachineFormat)
    {
        for (size_t idx = 0; idx < lines.size(); ++idx)
        {
            string current = lines[idx];
            if (current.empty())
                continue;

            stringstream ss(current);
            vector<string> tokens;
            string token;
            while (getline(ss, token, '|'))
                tokens.push_back(token);

            if (tokens.size() < 8)
                continue;

            deThi de;
            de.id = safeStoi(tokens[0]);
            de.tenMon = tokens[1];
            de.tenBaiThi = tokens[2];
            de.thoiGianLamBai = safeStoi(tokens[3]);
            de.loaiThi = safeStoi(tokens[4]);
            int soCau = safeStoi(tokens[5]);
            de.phongThiId = safeStoi(tokens[6]);

            string dsLopStr = "";
            if (tokens.size() >= 13)
            {
                de.batDau = parseDateTime(tokens[7]);
                de.ketThuc = parseDateTime(tokens[8]);
                de.locked = (tokens[9] == "1");
                de.soLanOnTap = safeStoi(tokens[10], -1);
                de.randomDe = (tokens[11] == "1");
                de.shuffleAns = (tokens[12] == "1");
                dsLopStr = (tokens.size() > 13 ? tokens[13] : "");
            }
            else if (tokens.size() >= 12)
            {
                de.batDau = tokens[7].empty() ? 0 : parseDateTime(tokens[7]);
                de.ketThuc = tokens[8].empty() ? 0 : parseDateTime(tokens[8]);
                de.locked = (tokens[9] == "1");
                de.soLanOnTap = safeStoi(tokens[10], -1);
                de.randomDe = (tokens[11] == "1");
                de.shuffleAns = false;
                if (tokens.size() > 12)
                    dsLopStr = tokens[12];
            }
            else
            {
                de.batDau = 0;
                de.ketThuc = 0;
                de.locked = false;
                de.soLanOnTap = -1;
                de.randomDe = false;
                de.shuffleAns = false;
                if (tokens.size() > 7)
                    dsLopStr = tokens[7];
            }

            if (dsLopStr != "ALL" && !dsLopStr.empty())
            {
                stringstream ssLop(dsLopStr);
                string tenLop;
                while (getline(ssLop, tenLop, ','))
                {
                    tenLop = trimString(tenLop);
                    if (!tenLop.empty())
                        de.dsLopDuocThi.push_back(tenLop);
                }
            }

            for (int i = 0; i < soCau; i++)
            {
                if (idx + 1 >= lines.size())
                    break;
                current = lines[++idx];
                if (!current.empty() && current.back() == '\r')
                    current.pop_back();

                stringstream s2(current);
                vector<string> chTk;
                string c;
                while (getline(s2, c, '|'))
                    chTk.push_back(c);
                if (chTk.size() < 7)
                    continue;

                CauHoi ch;
                ch.id = safeStoi(chTk[0]);
                ch.noiDung = chTk[1];
                ch.dapAn[0] = chTk[2];
                ch.dapAn[1] = chTk[3];
                ch.dapAn[2] = chTk[4];
                ch.dapAn[3] = chTk[5];
                ch.dapAnDung = safeStoi(chTk[6]);
                de.danhSachCauHoi.push_back(ch);
            }
            maDe.push_back(de);
        }
        return;
    }

    deThi readableDe;
    readableDe.id = 1;
    readableDe.tenMon = "";
    readableDe.tenBaiThi = "";
    readableDe.thoiGianLamBai = 0;
    readableDe.loaiThi = 1;
    readableDe.phongThiId = 0;
    readableDe.batDau = 0;
    readableDe.ketThuc = 0;
    readableDe.locked = false;
    readableDe.soLanOnTap = -1;
    readableDe.randomDe = false;
    readableDe.shuffleAns = false;
    readableDe.dsLopDuocThi.clear();
    readableDe.danhSachCauHoi.clear();

    size_t nextQuestionId = 1;
    bool foundAnyHeader = false;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        string trimmed = trimString(lines[i]);
        if (trimmed.empty())
            continue;

        if (beginsWith(trimmed, "Ma de thi:"))
        {
            string idText = extractFieldValue(trimmed, "Ma de thi:");
            int idValue = extractNumericSuffix(idText);
            if (idValue <= 0)
                idValue = 1;
            readableDe.id = idValue;
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "Ten bai thi:"))
        {
            readableDe.tenBaiThi = extractFieldValue(trimmed, "Ten bai thi:");
            readableDe.tenMon = readableDe.tenBaiThi;
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "TeacherID:"))
        {
            readableDe.phongThiId = safeStoi(extractFieldValue(trimmed, "TeacherID:"), 0);
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "Lop duoc thi:"))
        {
            readableDe.dsLopDuocThi = parseListSection(lines, i);
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "So cau hoi:"))
        {
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "Thoi gian lam bai:"))
        {
            string value = extractFieldValue(trimmed, "Thoi gian lam bai:");
            size_t pos = value.find("phut");
            if (pos != string::npos)
                value = trimString(value.substr(0, pos));
            readableDe.thoiGianLamBai = safeStoi(value, 0);
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "Ngay mo de:"))
        {
            string value = extractFieldValue(trimmed, "Ngay mo de:");
            if (value.empty() && i + 1 < lines.size())
                value = trimString(lines[i + 1]);
            readableDe.batDau = value.empty() ? 0 : parseDateTime(value);
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "Ngay dong de:"))
        {
            string value = extractFieldValue(trimmed, "Ngay dong de:");
            if (value.empty() && i + 1 < lines.size())
                value = trimString(lines[i + 1]);
            readableDe.ketThuc = value.empty() ? 0 : parseDateTime(value);
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "Random cau hoi:"))
        {
            readableDe.randomDe = parseYesNo(extractFieldValue(trimmed, "Random cau hoi:"));
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "Shuffle dap an:"))
        {
            readableDe.shuffleAns = parseYesNo(extractFieldValue(trimmed, "Shuffle dap an:"));
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "Cho phep thi lai:"))
        {
            readableDe.soLanOnTap = parseYesNo(extractFieldValue(trimmed, "Cho phep thi lai:")) ? 1 : -1;
            foundAnyHeader = true;
        }
        else if (beginsWith(trimmed, "Trang thai:"))
        {
            string value = extractFieldValue(trimmed, "Trang thai:");
            if (value.empty() && i + 1 < lines.size())
                value = trimString(lines[i + 1]);
            readableDe.locked = !parseYesNo(value);
            foundAnyHeader = true;
        }
    }

    for (size_t idx = 0; idx < lines.size(); ++idx)
    {
        string trimmed = trimString(lines[idx]);
        if (!beginsWith(trimmed, "[Cau"))
            continue;

        size_t cursor = idx + 1;
        while (cursor < lines.size() && trimString(lines[cursor]).empty())
            ++cursor;

        if (cursor >= lines.size())
            break;

        string questionText = trimString(lines[cursor++]);
        while (cursor < lines.size())
        {
            string nextLine = trimString(lines[cursor]);
            if (nextLine.empty())
            {
                ++cursor;
                continue;
            }
            if (beginsWith(nextLine, "A.") || beginsWith(nextLine, "B.") || beginsWith(nextLine, "C.") || beginsWith(nextLine, "D.") || beginsWith(nextLine, "Dap an dung:") || beginsWith(nextLine, "[Cau"))
                break;
            questionText += " " + nextLine;
            ++cursor;
        }

        string options[4] = {"", "", "", ""};
        while (cursor < lines.size())
        {
            string nextLine = trimString(lines[cursor]);
            if (nextLine.empty())
            {
                ++cursor;
                continue;
            }
            if (beginsWith(nextLine, "A."))
            {
                options[0] = trimString(nextLine.substr(2));
                ++cursor;
                continue;
            }
            if (beginsWith(nextLine, "B."))
            {
                options[1] = trimString(nextLine.substr(2));
                ++cursor;
                continue;
            }
            if (beginsWith(nextLine, "C."))
            {
                options[2] = trimString(nextLine.substr(2));
                ++cursor;
                continue;
            }
            if (beginsWith(nextLine, "D."))
            {
                options[3] = trimString(nextLine.substr(2));
                ++cursor;
                continue;
            }
            break;
        }

        int answerIndex = 0;
        while (cursor < lines.size())
        {
            string nextLine = trimString(lines[cursor]);
            if (nextLine.empty())
            {
                ++cursor;
                continue;
            }
            if (beginsWith(nextLine, "Dap an dung:"))
            {
                answerIndex = parseAnswerLetter(extractFieldValue(nextLine, "Dap an dung:"));
                ++cursor;
                break;
            }
            if (beginsWith(nextLine, "[Cau"))
                break;
            ++cursor;
        }

        CauHoi ch;
        ch.id = static_cast<int>(nextQuestionId++);
        ch.noiDung = questionText;
        ch.dapAn[0] = options[0];
        ch.dapAn[1] = options[1];
        ch.dapAn[2] = options[2];
        ch.dapAn[3] = options[3];
        ch.dapAnDung = answerIndex;
        readableDe.danhSachCauHoi.push_back(ch);
    }

    if (foundAnyHeader && !readableDe.danhSachCauHoi.empty())
    {
        if (readableDe.tenMon.empty())
            readableDe.tenMon = readableDe.tenBaiThi;
        maDe.push_back(readableDe);
    }
}

// ========================== STUDENT ==========================
class SinhVien : public Person
{
private:
    string lopChuNhiem;       // chỉ 1 lớp chủ nhiệm
    vector<string> monHoc;    // nhiều môn học
    vector<pair<string, double>> diemMon; // diem theo mon hoc
    string name;
    string gioitinh;
    string birthDay;
    string sdt;
    vector<KetQua> danhSachKetQua;
    double diemTB;
    int teacherId;


public:
    SinhVien() : Person()
    {
        lopChuNhiem = "";
        gioitinh = "";
        birthDay = "";
        sdt = "";
        diemTB = 0.0;
        teacherId = 0;
    }

    SinhVien(int id, string userN, string passW, string ten,
        string lopChuNhiem, string gioitinh, string birthDay, string sdt, double diemTB, int teacherId)
        : Person(id, userN, passW, ten), lopChuNhiem(lopChuNhiem), gioitinh(gioitinh), birthDay(birthDay), sdt(sdt), diemTB(diemTB), teacherId(teacherId)
    {
    }

    bool kiemTraLopDuocThi(const deThi& de) const
    {
        if (de.dsLopDuocThi.empty())
            return true;
        for (const string& l : de.dsLopDuocThi)
        {
            if (l == this->lopChuNhiem)
                return true;
        }
        return false;
    }

    // ===== GET/SET ID =====
    int getId() const
    {
        return id;
    }

    void setId(int x)
    {
        id = x;
    }

    // ===== GET/SET NAME =====
    string getName() const
    {
        return name;
    }

    void setName(const string& s)
    {
        name = s;
    }

    // ===== LOP CHU NHIEM =====
    string getLopChuNhiem() const
    {
        return lopChuNhiem;
    }

    void setLopChuNhiem(const string& lop)
    {
        lopChuNhiem = lop;
    }

    // ===== MON HOC =====
    void addMonHoc(const string& mon)
    {
        monHoc.push_back(mon);
    }

    vector<string> getMonHoc() const
    {
        return monHoc;
    }
    

    string getRole() const override { return "SinhVien"; }
    string getLop() const { return lopChuNhiem; }
    string getGioiTinh() const { return gioitinh; }
    string getBirthDay() const { return birthDay; }
    string getSdt() const { return sdt; }
    int layMaGV() const { return teacherId; }
    double layDiem() const { return diemTB; }

    void datDiem(double d) { diemTB = d; }
    void setlopChuNhiem(const string& lop) { this->lopChuNhiem = lop; }
    void setMonHoc(const vector<string>& monHoc) { this->monHoc = monHoc; }
    void addDiemMon(const string& mon, double diem) { diemMon.emplace_back(mon, diem); }
    void setDiemMon(const vector<pair<string, double>>& diemMon) { this->diemMon = diemMon; }
    const vector<pair<string, double>>& getDiemMon() const { return diemMon; }
    double tinhDiemTBTatCaMon() const
    {
        if (diemMon.empty()) return diemTB;
        double tong = 0;
        for (const auto& p : diemMon)
        {
            tong += p.second;
        }
        return round((tong / diemMon.size()) * 100.0) / 100.0;
    }
    string getHocLuc() const
    {
        double tb = tinhDiemTBTatCaMon();
        if (tb >= 8.0) return "Gioi";
        if (tb >= 6.5) return "Kha";
        if (tb >= 5.0) return "Trung binh";
        return "Yeu";
    }
    void setGioiTinh(const string& gioitinh) { this->gioitinh = gioitinh; }
    void setBirthDay(const string& birthDay) { this->birthDay = birthDay; }
    void setSdt(const string& sdt) { this->sdt = sdt; }
    void setDiemTB(double diemTB) { this->diemTB = diemTB; }
    void setTeacherId(int teacherId) { this->teacherId = teacherId; }
    void setDanhSachKetQua(const vector<KetQua>& ds) { danhSachKetQua = ds; }
    const vector<KetQua>& getDanhSachKetQua() const { return danhSachKetQua; }

    void hienThiThongTin() const
    {
        cout << "========== Thong Tin Sinh Vien ==========\n"
            << "[HS] " << fullName << " | ID: " << id << " | Lop: " << lopChuNhiem << "\n"
            << "Tai khoan: " << username << " | Mat khau: (*******)\n"
            << "Gioi tinh: " << gioitinh << " | Ngay sinh: " << birthDay << " | SDT: " << sdt << "\n";

        if (!diemMon.empty())
        {
            cout << "| Diem mon:\n";
            for (const auto& p : diemMon)
            {
                cout << "    " << p.first << ": " << fixed << setprecision(2) << p.second << "\n";
            }
        }
        cout << "| Diem TB tat ca mon: " << fixed << setprecision(2) << tinhDiemTBTatCaMon() << "\n"
             << "| Hoc luc: " << getHocLuc() << " | TeacherID: " << teacherId << endl;
    }

    void xemDanhSachDe(QuanLyThi& thi, int loaiBai)
    {
        SetColor(LIGHT_CYAN);
        cout << (loaiBai == 1 ? "\n--- DANH SACH BAI THI CHINH THUC ---\n" : "\n--- DANH SACH BAI ON TAP ---\n");
        SetColor(WHITE);

        bool coBai = false;

        for (const auto& de : thi.getMaDe())
        {
            if (!kiemTraLopDuocThi(de) || de.loaiThi != loaiBai)
                continue;

            bool daThiChinhThuc = false;
            if (de.loaiThi == 1)
            {
                for (const auto& kq : danhSachKetQua)
                {
                    if (kq.maDe == de.id && kq.loaiThi == 1)
                    {
                        daThiChinhThuc = true;
                        break;
                    }
                }
            }
            if (!daThiChinhThuc)
            {
                string loaiStr = (de.loaiThi == 1) ? "[CHINH THUC (1 lan)]" : "[ON TAP (n lan)]";
                string lopInfo;
                if (de.dsLopDuocThi.empty())
                    lopInfo = "ALL";
                else
                {
                    for (size_t i = 0; i < de.dsLopDuocThi.size(); i++)
                    {
                        lopInfo += de.dsLopDuocThi[i];
                        if (i + 1 < de.dsLopDuocThi.size())
                            lopInfo += ", ";
                    }
                }
                cout << "Ten bai thi: " << de.tenBaiThi
                    << " | Mon: " << de.tenMon << " " << loaiStr
                    << " | Thoi gian: " << de.thoiGianLamBai << " phut"
                    << " | Bat dau: " << (de.batDau == 0 ? string("Chua co") : formatDateTime(de.batDau))
                    << " | Ket thuc: " << (de.ketThuc == 0 ? string("Chua co") : formatDateTime(de.ketThuc))
                    << " | Lop: " << lopInfo << "\n";
                coBai = true;
            }
        }
        if (!coBai)
        {
            SetColor(YELLOW);
            cout << (loaiBai == 1 ? "Hien tai khong co bai thi chinh thuc nao!\n" : "Hien tai khong co bai on tap nao!\n");
            SetColor(WHITE);
        }
    }

    void xemThongTinDe(QuanLyThi& thi, int loaiBai)
    {
        string tenBai = inputLine("Nhap ten bai thi can xem: ");
        for (auto& de : thi.getMaDe())
        {
            if (de.tenBaiThi == tenBai && de.loaiThi == loaiBai)
            {
                if (!kiemTraLopDuocThi(de))
                {
                    cout << "Ban khong co quyen xem thong tin de thi nay!\n";
                    return;
                }
                cout << "\n===== THONG TIN CHI TIET =====\n";
                cout << "Ma de: " << de.id << endl;
                cout << "Ten bai thi: " << de.tenBaiThi << endl;
                cout << "Mon: " << de.tenMon << endl;
                cout << "So cau hoi: " << de.danhSachCauHoi.size() << endl;
                cout << "Thoi gian: " << de.thoiGianLamBai << " phut\n";
                cout << "Trang thai: " << (de.locked ? "Dang khoa" : "Dang mo") << endl;
                if (de.loaiThi == 1)
                    cout << "Lich thi: " << de.getSchedule() << endl;
                else
                    cout << "So lan lam toi da: " << (de.soLanOnTap == -1 ? "Khong gioi han" : to_string(de.soLanOnTap)) << endl;
                return;
            }
        }
        cout << "Khong tim thay bai thi nay trong danh sach!\n";
    }

    double traLoiCauHoi(vector<CauHoi> danhSachCauHoi, time_t start, int thoiGian) const
    {
        startExamTimer(thoiGian);
        int n;
        double diemThi = 0;
        for (auto& ch : danhSachCauHoi)
        {
            time_t now = time(0);
            if (difftime(now, start) >= thoiGian)
            {
                cout << "\nHet gio lam bai!\n";
                break;
            }
            int remaining = thoiGian - (int)difftime(now, start);
            int hours = remaining / 3600;
            int minutes = (remaining % 3600) / 60;
            int secs = remaining % 60;
            cout << "\n[Dong ho] Con lai: " << setfill('0') << setw(2) << hours << ":"
                 << setw(2) << minutes << ":" << setw(2) << secs << "\n";
            cout << "Cau " << ch.id << ". " << ch.noiDung << endl;
            for (int i = 0; i < 4; i++)
            {
                cout << i + 1 << ". " << ch.dapAn[i] << endl;
            }
            n = nhapSo<int>("Dap an ban chon: ");
            if (n == ch.dapAnDung)
            {
                diemThi += 10.0 / danhSachCauHoi.size();
            }
        }
        stopExamTimer();
        return diemThi;
    }

    bool verifyStudentInfo()
    {
        SetColor(LIGHT_CYAN);
        cout << "\n===== XAC THUC THONG TIN =====\n";
        SetColor(WHITE);
        
        int nhapID = nhapSo<int>("Nhap ID sinh vien: ");
        string nhapLop = inputLine("Nhap lop: ");
        
        if (nhapID != this->id)
        {
            SetColor(RED);
            cout << "ID khong dung!\n";
            SetColor(WHITE);
            return false;
        }
        
        if (nhapLop != this->lopChuNhiem)
        {
            SetColor(RED);
            cout << "Lop khong dung!\n";
            SetColor(WHITE);
            return false;
        }
        
        SetColor(GREEN);
        cout << "Xac thuc thanh cong! Chao " << this->fullName << "\n";
        SetColor(WHITE);
        return true;
    }

    void lamBai(QuanLyThi& thi, int loaiBai)
    {
        if (!verifyStudentInfo()) {
            return;
        }
        
        cout << "\n===== CHON BAI THI =====\n";
        xemDanhSachDe(thi, loaiBai);
        
        string tenBaiThi = inputLine("Nhap ten bai thi de chon: ");
        deThi* deHienTai = nullptr;
        for (auto& de : thi.getMaDe())
        {
            if (de.tenBaiThi == tenBaiThi && de.loaiThi == loaiBai && kiemTraLopDuocThi(de))
            {
                deHienTai = const_cast<deThi*>(&de);
                break;
            }
        }

        if (deHienTai == nullptr || deHienTai->danhSachCauHoi.empty())
        {
            SetColor(RED);
            cout << "De khong hop le hoac chua co cau hoi!\n";
            SetColor(WHITE);
            return;
        }
        if (!kiemTraLopDuocThi(*deHienTai))
        {
            SetColor(RED);
            cout << "Ban khong thuoc danh sach lop duoc phep lam bai nay!\n";
            SetColor(WHITE);
            return;
        }
        if (deHienTai->locked)
        {
            SetColor(RED);
            cout << "Bai dang bi khoa!\n";
            SetColor(WHITE);
            return;
        }
        if (loaiBai == 1 && !deHienTai->isAvailable())
        {
            SetColor(YELLOW);
            if (deHienTai->batDau != 0 && time(0) < deHienTai->batDau)
            {
                cout << "Chua den gio. Lich thi: " << deHienTai->getSchedule() << "\n";
            }
            else if (deHienTai->ketThuc != 0 && time(0) > deHienTai->ketThuc)
            {
                cout << "Da ket thuc. Lich thi: " << deHienTai->getSchedule() << "\n";
            }
            else
            {
                cout << "Bai thi hien tai chua duoc mo. Lich thi: " << deHienTai->getSchedule() << "\n";
            }
            SetColor(WHITE);
            return;
        }

        int soLanDaThi = 0;
        for (const auto& kq : danhSachKetQua)
        {
            if (kq.maDe == deHienTai->id)
                soLanDaThi++;
        }

        if (loaiBai == 1)
        {
            if (soLanDaThi >= 1)
            {
                SetColor(RED);
                cout << "Bai thi Chinh thuc chi duoc lam 1 lan. Ban da nop bai roi!\n";
                SetColor(WHITE);
                return;
            }
            SetColor(LIGHT_GREEN);
            cout << "Bat dau lam bai thi chinh thuc.\n";
            SetColor(WHITE);
        }
        else
        {
            if (deHienTai->soLanOnTap != -1 && soLanDaThi >= deHienTai->soLanOnTap)
            {
                SetColor(RED);
                cout << "Ban da lam bai on tap nay " << soLanDaThi << "/"
                    << deHienTai->soLanOnTap << " lan. Het luot!\n";
                SetColor(WHITE);
                return;
            }

            SetColor(LIGHT_CYAN);
            if (deHienTai->soLanOnTap == -1)
                cout << "Lan lam thu " << soLanDaThi + 1 << " (Khong gioi han)\n";
            else
                cout << "Lan lam thu " << soLanDaThi + 1
                << "/" << deHienTai->soLanOnTap << "\n";
            SetColor(WHITE);
        }

        cout << "\n=== BAT DAU LAM BAI: " << deHienTai->tenBaiThi << " ===\n";
        SetColor(WHITE);
        time_t start = time(0);

        // Random câu hỏi nếu cần
        vector<CauHoi> cauHoiLam = deHienTai->danhSachCauHoi;
        if (deHienTai->randomDe) {
            cauHoiLam = shuffleCauHoi(cauHoiLam);
            SetColor(LIGHT_YELLOW);
            cout << "Cau hoi da duoc sap xep ngau nhien.\n";
            SetColor(WHITE);
        }
        
        // Shuffle đáp án nếu cần
        if (deHienTai->shuffleAns) {
            for (auto& ch : cauHoiLam) {
                shuffleDapAn(ch);
            }
            SetColor(LIGHT_YELLOW);
            cout << "Dap an da duoc xao tron.\n";
            SetColor(WHITE);
        }

        double diemThi = traLoiCauHoi(cauHoiLam, start, deHienTai->thoiGianLamBai * 60);

        int thoiGianThucTe = (int)difftime(time(0), start);
        if (thoiGianThucTe == 0) thoiGianThucTe = 1;

        SetColor(GREEN);
        cout << "=> Nop bai thanh cong! Diem: " << fixed << setprecision(1) << diemThi 
             << "/10\n";
        SetColor(WHITE);

        KetQua kq;
        kq.idSV = this->id;
        kq.tenSV = this->fullName;
        kq.diem = diemThi;
        kq.mon = deHienTai->tenMon;
        kq.thoiGian = thoiGianThucTe;
        kq.maDe = deHienTai->id;
        kq.loaiThi = deHienTai->loaiThi;
        this->danhSachKetQua.push_back(kq);

        if (loaiBai == 2)
        {
            SetColor(LIGHT_CYAN);
            cout << "\n===== DAP AN =====\n";
            for (const auto& ch : deHienTai->danhSachCauHoi)
            {
                cout << ch.id << ". Dap an dung: " << ch.dapAnDung << ". " << ch.dapAn[ch.dapAnDung - 1] << "\n";
            }
            SetColor(WHITE);
        }
    }

    void hienThiDanhSachKetQua() const
    {
        cout << "\n===== LICH SU LAM BAI =====\n";
        if (danhSachKetQua.empty())
        {
            cout << "Chua co ket qua nao!\n";
            return;
        }
        for (const auto& kq : danhSachKetQua)
        {
            string loai = (kq.loaiThi == 1) ? "[Chinh thuc]" : "[On tap]";
            cout << "Ma de: " << kq.maDe << " " << loai << " | Mon: " << kq.mon << " | Diem: " << kq.diem << "/10\n";
        }
    }

    void thongKeDiemTB()
    {
        double tong = 0;
        int dem = 0;
        for (const auto& kq : danhSachKetQua)
        {
            if (kq.loaiThi == 1)
            {
                tong += kq.diem;
                dem++;
            }
        }
        cout << "\n===== THONG KE DIEM =====\n";
        if (dem == 0)
        {
            cout << "Ban chua co diem bai thi Chinh thuc nao!\n";
        }
        else
        {
            cout << "Tong so bai da thi (Chinh thuc): " << dem << "\n";
            cout << "Diem TB tich luy: " << tong / dem << "\n";
        }
    }

    void xemBangXepHang(const vector<SinhVien>& dsSinhVien)
    {
        SetColor(CYAN);
        cout << "\n===== BANG XEP HANG TOAN TRUONG (CHINH THUC) =====\n";
        SetColor(WHITE);
        struct HSScore {
            string fullName;
            double score;
        };
        vector<HSScore> bxh;

        for (const auto& hs : dsSinhVien)
        {
            double tong = 0;
            int dem = 0;
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                if (kq.loaiThi == 1)
                {
                    tong += kq.diem;
                    dem++;
                }
            }
            if (dem > 0)
                bxh.push_back({ hs.getFullName(), tong / dem });
        }

        if (bxh.empty())
        {
            SetColor(YELLOW);
            cout << "Chua co du lieu de xep hang!\n";
            SetColor(WHITE);
            return;
        }

        sort(bxh.begin(), bxh.end(), [](const HSScore& a, const HSScore& b) { return a.score > b.score; });

        for (int i = 0; i < (int)bxh.size(); i++)
        {
            if (bxh[i].fullName == this->fullName) SetColor(LIGHT_GREEN);
            cout << i + 1 << ". Ho ten: " << bxh[i].fullName << " | Diem TB: " << bxh[i].score << '\n';
            SetColor(WHITE);
        }
    }

    void xemBangXepHangTheoBaiThi(const QuanLyThi& thi, const vector<SinhVien>& dsSinhVien)
    {
        int maDe = nhapSo<int>("Nhap Ma De can xem xep hang: ");
        const deThi* de = thi.timDeThi(maDe);
        
        if (!de) {
            SetColor(RED);
            cout << "Ma de khong ton tai!\n";
            SetColor(WHITE);
            return;
        }

        SetColor(CYAN);
        cout << "\n===== BANG XEP HANG BAI THI: " << de->tenBaiThi << " =====\n";
        SetColor(WHITE);

        struct HSScore {
            string fullName;
            double score;
        };
        vector<HSScore> bxh;

        for (const auto& hs : dsSinhVien)
        {
            if (!hs.kiemTraLopDuocThi(*de)) continue;
            
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                if (kq.maDe == maDe)
                {
                    bxh.push_back({ hs.getFullName(), kq.diem });
                    break;
                }
            }
        }

        if (bxh.empty())
        {
            SetColor(YELLOW);
            cout << "Chua co ai lam bai thi nay!\n";
            SetColor(WHITE);
            return;
        }

        sort(bxh.begin(), bxh.end(), [](const HSScore& a, const HSScore& b) { return a.score > b.score; });

        for (int i = 0; i < (int)bxh.size(); i++)
        {
            if (bxh[i].fullName == this->fullName) SetColor(LIGHT_GREEN);
            cout << i + 1 << ". Ho ten: " << bxh[i].fullName << " | Diem: " << fixed << setprecision(2) << bxh[i].score << '\n';
            SetColor(WHITE);
        }
    }

    void datLaiMatKhau()
    {
        string mk;
        cout << "Nhap mat khau moi: ";
        getline(cin, mk);
        password = mk;
        SetColor(GREEN);
        cout << "Doi mat khau thanh cong!\n";
        SetColor(WHITE);
    }

    string chuyenThanhChuoiFile() const
    {
        return to_string(id) + "|" + username + "|" + password + "|" + fullName + "|" + lopChuNhiem + "|" + to_string(diemTB) + "|" + to_string(teacherId);
    }

    void menu(QuanLyThi& thi, const vector<SinhVien>& dsSinhVienToanTruong)
    {
        int choice;
        do
        {
            SetColor(CYAN);
            cout << "\n╔══════════════════════════════════════════════╗\n";
            cout << "║               MENU SINH VIEN                 ║\n";
            cout << "╠══════════════════════════════════════════════╣\n";

            SetColor(YELLOW);
            cout << "║  1. Xem thong tin ca nhan                    ║\n";
            cout << "║  2. Xem danh sach bai thi                    ║\n";
            cout << "║  3. Xem thong tin bai thi                    ║\n";
            cout << "║  4. Lam bai thi                              ║\n";
            cout << "║  5. Xem lich su thi                          ║\n";
            cout << "║  6. Thong ke diem trung binh cua bai thi     ║\n";
            cout << "║  7. Xem bang xep hang                        ║\n";
            cout << "║  8. Xem danh sach bai on tap                 ║\n";
            cout << "║  9. Xem thong tin bai on tap                 ║\n";
            cout << "║ 10. Lam bai on tap                           ║\n";
            cout << "║ 11. Doi mat khau                             ║\n";
            cout << "║  0. Dang xuat                                ║\n";

            SetColor(CYAN);
            cout << "╚══════════════════════════════════════════════╝\n";

            SetColor(GREEN);
            choice = nhapSo<int>("Chon chuc nang: ");

            SetColor(WHITE);
            switch (choice)
            {
            case 1: hienThiThongTin(); break;
            case 2: xemDanhSachDe(thi, 1); break;
            case 3: xemThongTinDe(thi, 1); break;
            case 4: lamBai(thi, 1); break;
            case 5: hienThiDanhSachKetQua(); break;
            case 6: thongKeDiemTB(); break;
            case 7: xemBangXepHang(dsSinhVienToanTruong); break;
            case 8: xemDanhSachDe(thi, 2); break;
            case 9: xemThongTinDe(thi, 2); break;
            case 10: lamBai(thi, 2); break;
            case 11:
                datLaiMatKhau();
                break;
            case 0:
                SetColor(GREEN);
                cout << "Dang xuat thanh cong!\n";
                SetColor(WHITE);
                break;
            default:
                SetColor(RED);
                cout << "Lua chon khong hop le!\n";
                SetColor(WHITE);
            }
        } while (choice != 0);
    }
};

// ========================== TEACHER ==========================
class GiangVien : public Person
{
private:
    string mon;
    string teacherRole;  // comma-separated roles: "BoMon,CoVan" or "BoMon" or "CoVan"
    string advisorClass;

    // Parse roles from comma-separated string
    vector<string> parseRoles() const {
        vector<string> roles;
        stringstream ss(teacherRole);
        string role;
        while (getline(ss, role, ',')) {
            if (!role.empty()) {
                roles.push_back(role);
            }
        }
        return roles;
    }

public:
    void setMon(const string& m) { mon = m; }
    string getMon() const { return mon; }
    void setTeacherRole(const string& r) { teacherRole = r; }
    string getTeacherRole() const { return teacherRole; }
    void setAdvisorClass(const string& c) { advisorClass = c; }
    string getAdvisorClass() const { return advisorClass; }
    
    bool isAdvisor() const {
        vector<string> roles = parseRoles();
        for (const auto& r : roles) {
            if (r == "CoVan" || r == "CoVanHocTap") return true;
        }
        return false;
    }
    
    bool isSubjectTeacher() const {
        vector<string> roles = parseRoles();
        for (const auto& r : roles) {
            if (r == "BoMon") return true;
        }
        return false;
    }
    
    bool hasRole(const string& role) const {
        vector<string> roles = parseRoles();
        for (const auto& r : roles) {
            if (r == role) return true;
        }
        return false;
    }

    void normalizeTeacherRole() {
        bool hasBoMon = false;
        bool hasCoVan = false;
        vector<string> roles = parseRoles();
        for (const auto& r : roles) {
            if (r == "BoMon") hasBoMon = true;
            if (r == "CoVan" || r == "CoVanHocTap") hasCoVan = true;
        }
        if (!hasBoMon && hasCoVan)
            hasBoMon = true;

        teacherRole.clear();
        teacherRole = "BoMon";
        if (hasCoVan)
            teacherRole += ",CoVan";
    }
    
    string getTeacherRoleLabel() const {
        vector<string> roles = parseRoles();
        string label = "";
        for (size_t i = 0; i < roles.size(); i++) {
            if (roles[i] == "BoMon") label += (label.empty() ? "BoMon" : ",BoMon");
            else if (roles[i] == "CoVan" || roles[i] == "CoVanHocTap") {
                label += (label.empty() ? "CoVan" : ",CoVan");
            }
        }
        return label.empty() ? "BoMon" : label;
    }

    GiangVien() : Person(), teacherRole("BoMon"), advisorClass("") {};
    GiangVien(int id, string u, string p, string ten, string mon, string role = "BoMon")
        : Person(id, u, p, ten), mon(mon), teacherRole(role), advisorClass("") {}

    string getRole() const override
    {
        return "GiangVien";
    }

    void hienThiThongTin() const
    {
        SetColor(LIGHT_YELLOW);
        string roleDisplay = "[";
        if (isSubjectTeacher()) roleDisplay += "GVBM";
        if (isSubjectTeacher() && isAdvisor()) roleDisplay += "+";
        if (isAdvisor()) roleDisplay += "CVHT";
        roleDisplay += "]";
        
        cout << roleDisplay << " " << fullName
             << " | Mon: " << mon
             << " | Vai tro: " << getTeacherRoleLabel();
        if (!advisorClass.empty())
            cout << " | Lop chu nhiem: " << advisorClass;
        cout << endl;

        SetColor(WHITE);
    }

    string chuyenThanhChuoiFile() const
    {
        return to_string(id) + "|" + username + "|" + password + "|" + fullName + "|" + mon + "|" + getTeacherRoleLabel() + "|" + advisorClass;
    }

    void datLaiMatKhau()
    {
        string mkCu = inputLine("Nhap mat khau cu: ");
        if (mkCu != this->password)
        {
            SetColor(RED);
            cout << "Mat khau cu khong dung!\n";
            SetColor(WHITE);
            return;
        }
        string mkMoi = inputLine("Nhap mat khau moi: ");
        string mkMoiXacNhan = inputLine("Xac nhan mat khau moi: ");
        if (mkMoi != mkMoiXacNhan)
        {
            SetColor(RED);
            cout << "Xac nhan mat khau khong dung!\n";
            SetColor(WHITE);
            return;
        }
        this->password = mkMoi;
        SetColor(GREEN);
        cout << "Thay doi mat khau thanh cong!\n";
        SetColor(WHITE);
    }
    void xemSinhVien(const vector<SinhVien>& students, const vector<ClassInfo>& classes)
    {
        SetColor(LIGHT_CYAN);
        cout << "\n--- Danh sach sinh vien giang vien day ---\n";
        SetColor(WHITE);

        // Get classes this teacher is involved with (advisor or subject teacher)
        vector<string> classNames = layDanhSachLopDay(classes);

        for (const auto& hs : students)
        {
            bool show = false;
            if (hs.layMaGV() == this->id) show = true;
            if (!show && !classNames.empty() && find(classNames.begin(), classNames.end(), hs.getLop()) != classNames.end()) show = true;

            if (show) hs.hienThiThongTin();
        }
    }

    vector<string> layDanhSachLopDay(const vector<ClassInfo>& classes) const
    {
        vector<string> result;
        for (const auto& c : classes)
        {
            if (c.coVanHocTapId == id || find(c.danhSachGiangVien.begin(), c.danhSachGiangVien.end(), id) != c.danhSachGiangVien.end())
            {
                result.push_back(c.className);
            }
        }
        sort(result.begin(), result.end());
        result.erase(unique(result.begin(), result.end()), result.end());
        return result;
    }

    void xemKetQuaThi(const vector<SinhVien>& students, const vector<ClassInfo>& classes)
    {
        SetColor(LIGHT_CYAN);
        cout << "\n===== KET QUA THI =====\n";

        vector<string> classNames = layDanhSachLopDay(classes);
        bool hasAdvisorClass = false;
        for (const auto& c : classes)
        {
            if (c.coVanHocTapId == id)
            {
                hasAdvisorClass = true;
                break;
            }
        }
        bool isAdvisorRole = isAdvisor() || hasAdvisorClass;

        if (isAdvisorRole)
        {
            cout << "--- Giao vien co van hoc tap xem ket qua toan lop ---\n";
            if (classNames.empty())
            {
                cout << "Chua co lop duoc phan cong cho giao vien nay.\n";
            }
            for (const auto& hs : students)
            {
                if (!classNames.empty() && find(classNames.begin(), classNames.end(), hs.getLop()) == classNames.end())
                    continue;

                SetColor(LIGHT_YELLOW);
                cout << "\nSinh vien: " << hs.getFullName() << " | Lop: " << hs.getLop() << endl;

                for (const auto& kq : hs.getDanhSachKetQua())
                {
                    SetColor(LIGHT_GREEN);
                    cout << "Mon: " << kq.mon
                        << " | Diem: " << kq.diem
                        << " | Thoi gian: " << kq.thoiGian << "s\n";
                }
            }
        }
        else
        {
            cout << "--- Giao vien bo mon xem ket qua mon " << mon << " ---\n";
            if (classNames.empty())
            {
                cout << "Chua duoc phan cong lop. Se hien thi ket qua mon cua tat ca sinh vien.\n";
            }
            for (const auto& hs : students)
            {
                if (!classNames.empty() && find(classNames.begin(), classNames.end(), hs.getLop()) == classNames.end())
                    continue;

                bool any = false;
                for (const auto& kq : hs.getDanhSachKetQua())
                {
                    if (kq.mon == mon && kq.loaiThi == 1)
                    {
                        if (!any)
                        {
                            SetColor(LIGHT_YELLOW);
                            cout << "\nSinh vien: " << hs.getFullName() << " | Lop: " << hs.getLop() << endl;
                            any = true;
                        }

                        SetColor(LIGHT_GREEN);
                        cout << "Mon: " << kq.mon
                            << " | Diem: " << kq.diem
                            << " | Thoi gian: " << kq.thoiGian << "s\n";
                    }
                }
            }
        }

        SetColor(WHITE);
    }



    void sapXepSinhVienTheoDiem(vector<SinhVien>& students)
    {
        vector<SinhVien> ds;
        for (auto hs : students)
        {
            if (hs.layMaGV() == this->id)
            {
                ds.push_back(hs);
            }
        }
        sort(ds.begin(), ds.end(), [](SinhVien a, SinhVien b)
            { return a.layDiem() > b.layDiem(); });
        SetColor(CYAN);
        cout << "\n===== SAP XEP sinh vien THEO DIEM =====\n";
        SetColor(WHITE);
        for (auto& hs : ds)
        {
            cout << hs.getFullName() << " | Diem TB: " << hs.layDiem() << endl;
        }
    }

    void thongKe(const vector<SinhVien>& students)
    {
        double tong = 0;
        int dem = 0;
        for (const auto& hs : students)
        {
            if (hs.layMaGV() == this->id)
            {
                tong += hs.layDiem();
                dem++;
            }
        }
        if (dem)
            cout << "Diem TB: " << tong / dem << endl;
        else
            cout << "Khong co du lieu!\n";
    }

    void thongKeTheoMon(const vector<SinhVien>& students, const vector<ClassInfo>& classes)
    {
        string monTK = inputLine("Nhap ten mon muon thong ke: ");
        SetColor(CYAN);

        cout << "\n--- THONG KE DIEM MON "
            << monTK
            << " (CHINH THUC) ---\n";

        SetColor(WHITE);
        struct HSData
        {
            string ten;
            double diem;
        };
        vector<HSData> tk;
        vector<string> classNames = layDanhSachLopDay(classes);
        bool isAdvisorRole = isAdvisor();

        if (!isAdvisorRole && monTK != mon)
        {
            SetColor(RED);
            cout << "Chi giao vien bo mon co the xem diem mon: " << mon << "\n";
            SetColor(WHITE);
            return;
        }

        if (classNames.empty())
        {
            SetColor(YELLOW);
            cout << "Giao vien nay chua duoc phan cong lop nao.\n";
            SetColor(WHITE);
            return;
        }

        for (const auto& hs : students)
        {
            if (find(classNames.begin(), classNames.end(), hs.getLop()) == classNames.end())
                continue;

            double maxDiem = -1;
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                if (kq.mon == monTK && kq.loaiThi == 1)
                    maxDiem = max(maxDiem, kq.diem);
            }
            if (maxDiem >= 0)
                tk.push_back({ hs.getFullName(), maxDiem });
        }

        if (tk.empty())
        {
            SetColor(YELLOW);
            cout << "Chua co du lieu thi Chinh thuc mon nay!\n";
            SetColor(WHITE);
        }
        else
        {
            sort(tk.begin(), tk.end(), [](const HSData& a, const HSData& b)
                { return a.diem > b.diem; });
            for (const auto& data : tk)
                cout << "HS: " << data.ten << " | Diem mon: " << data.diem << "\n";
        }
    }

    void menu(vector<SinhVien>& students, const vector<ClassInfo>& classes, const vector<PhongThi>& examRooms, QuanLyThi& thi)
    {
        int choice;
        do
        {
            SetColor(CYAN);
            cout << "╔══════════════════════════════════════════════╗\n";
            cout << "║               MENU GIANG VIEN                ║\n";
            cout << "╠══════════════════════════════════════════════╣\n";

            SetColor(YELLOW);
            cout << "║  1. Xem danh sach sinh vien                  ║\n";
            cout << "║  2. Thong ke diem trung binh                 ║\n";
            cout << "║  3. Quan ly de thi                           ║\n";
            cout << "║  4. Thong ke theo mon                        ║\n";
            cout << "║  5. Xem ket qua thi                          ║\n";
            cout << "║  6. Sap xep sinh vien theo diem              ║\n";
            cout << "║  7. Doi mat khau                             ║\n";
            cout << "║  0. Dang xuat                                ║\n";

            SetColor(CYAN);
            cout << "╚══════════════════════════════════════════════╝\n";

            SetColor(GREEN);
            choice = nhapSo<int>("Chon chuc nang: ");

            SetColor(WHITE);

            switch (choice)
            {
            case 1:
                SetColor(LIGHT_CYAN);
                xemSinhVien(students, classes);
                SetColor(WHITE);
                break;

            case 2:
                SetColor(LIGHT_YELLOW);
                thongKe(students);
                SetColor(WHITE);
                break;

            case 3:
                SetColor(LIGHT_MAGENTA);
                thi.menu(this->id, this->mon, classes, examRooms);
                SetColor(WHITE);
                break;

            case 4:
                SetColor(LIGHT_BLUE);
                thongKeTheoMon(students, classes);
                SetColor(WHITE);
                break;

            case 5:
                SetColor(LIGHT_GREEN);
                xemKetQuaThi(students, classes);
                SetColor(WHITE);
                break;

            case 6:
                SetColor(YELLOW);
                sapXepSinhVienTheoDiem(students);
                SetColor(WHITE);
                break;

            case 7:
                datLaiMatKhau();
                SetColor(GREEN);
                cout << "Doi mat khau thanh cong!\n";
                SetColor(WHITE);
                break;

            case 0:
                SetColor(LIGHT_RED);
                cout << "Dang xuat thanh cong!\n";
                SetColor(WHITE);
                break;

            default:
                SetColor(RED);
                cout << "Lua chon khong hop le!\n";
                SetColor(WHITE);
            }

        } while (choice != 0);
    }
};

// ========================== QUAN LY USER ==========================
class QuanLyNguoiDung
{
private:
    vector<Person*> users;

public:
    void them(Person* p) { users.push_back(p); }

    Person* dangNhap()
    {
        string u, p;
        cout << "User: ";
        cin >> u;
        cout << "Pass: ";
        cin >> p;
        cin.ignore();

        for (auto user : users)
        {
            if (user->getUsername() == u && user->getPassword() == p)
            {
                return user;
            }
        }
        return NULL;
    }

    vector<Person*>& layDanhSach() { return users; }
};

// ========================== RANK ITEM ==========================
struct RankItem
{
    int id;
    string fullName;
    double score;
};

// ========================== ADMIN SYSTEM ==========================
class AdminSystem
{
private:
    Admin adminAccount;
    vector<GiangVien> teachers;
    vector<SinhVien> students;
    vector<ClassInfo> classes;
    vector<PhongThi> examRooms;
    vector<RankItem> ranking;
    int totalStudents;
public:
    AdminSystem() : totalStudents(0) { sortRanking(); }

    vector<ClassInfo>& getClasses() { return classes; }
    vector<PhongThi>& getRooms() { return examRooms; }

    void sortRanking()
    {
        sort(ranking.begin(), ranking.end(), [](const RankItem& a, const RankItem& b)
            {
                if (a.score != b.score) return a.score > b.score;
                return a.fullName < b.fullName;
            });
    }

    void assignStudentToClass(SinhVien& student, int selectedLopId)
    {
        if (!student.getLop().empty()) {
            cout << "Sinh viên này đã thuộc biên chế lớp khác!\n";
            return;
        }

        int classIdx = findClassIndexById(selectedLopId);
        if (classIdx == -1) {
            cout << "Lop khong ton tai.\n";
            return;
        }

        ClassInfo& c = classes[classIdx];
        student.setlopChuNhiem(c.className);
        student.setTeacherId(c.coVanHocTapId);
        c.studentCount++;

        cout << "Da phan lop cho sinh vien " << student.getFullName() << " vao lop " << c.className << ".\n";
    }
    // ==================== VALIDATION ====================
    bool isValidName(const string& s) const
    {
        if (s.empty())
            return false;
        for (char c : s)
        {
            unsigned char uc = static_cast<unsigned char>(c);
            if (!(isalnum(uc) || isspace(uc)))
            {
                return false;
            }
        }
        return true;
    }

    bool isValidUsername(const string& s) const
    {
        if (s.empty())
            return false;
        for (unsigned char uc : s)
        {
            
         if (!isalpha(uc))
            {
                return false;
            }
        }
        return true;
    }

    bool isValidPassword(const string& s) const
    {
        return s.length() >= 6;
    }
    bool isValidDate(const string& date)
    {
        regex pattern("^\\d{2}/\\d{2}/\\d{4}$");

        if (!regex_match(date, pattern))
            return false;

        int day = stoi(date.substr(0, 2));
        int month = stoi(date.substr(3, 2));
        int year = stoi(date.substr(6, 4));

        if (month < 1 || month > 12)
            return false;

        int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    // kiểm tra năm nhuận
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        {
            daysInMonth[1] = 29;
        }

        if (day < 1 || day > daysInMonth[month - 1])
            return false;

        return true;
    }
    bool isValidPhone(const string& phone) const
    {
        if (phone.length() != 10)
            return false;

        for (char c : phone)
        {
            if (!isdigit(c))
                return false;
        }

        return true;
    }

    bool teacherIdExists(int id) const
    {
        for (const auto& t : teachers)
        {
            if (t.getId() == id)
                return true;
        }
        return false;
    }
    bool isTeacherAlreadyAdvisor(
    int teacherId,
    const string& ignoreClassId = string()
    ) const
    {
        for (const auto& c : classes)
        {
            if (!ignoreClassId.empty() && c.id == ignoreClassId)                
            continue;

            if (c.coVanHocTapId == teacherId)
                return true;
        }
        return false;
    }

    bool studentIdExists(int id) const
    {
        for (const auto& hs : students)
        {
            if (hs.getId() == id)
                return true;
        }
        return false;
    }

    bool classIdExists(const string& id) const
    {
        for (const auto& c : classes)
        {
            if (c.id == id)
                return true;
        }
        return false;
    }

    bool usernameExists(const string& username) const
    {
        if (username == "admin")
            return true;
        for (const auto& t : teachers)
        {
            if (t.getUsername() == username)
                return true;
        }
        for (const auto& hs : students)
        {
            if (hs.getUsername() == username)
                return true;
        }
        return false;
    }

    int findTeacherIndexById(int id) const
    {
        for (int i = 0; i < (int)teachers.size(); i++)
        {
            if (teachers[i].getId() == id)
                return i;
        }
        return -1;
    }

    string getTeacherNameById(int id) const
    {
        int idx = findTeacherIndexById(id);
        return idx == -1 ? "N/A" : teachers[idx].getFullName();
    }

    bool isSubjectTeacherId(int id) const
    {
        int idx = findTeacherIndexById(id);
        return idx != -1 && teachers[idx].isSubjectTeacher();
    }

    int findStudentIndexById(int id) const
    {
        for (int i = 0; i < (int)students.size(); i++)
        {
            if (students[i].getId() == id)
                return i;
        }
        return -1;
    }

    int findClassIndexById(int id) const
    {
        return findClassIndexById(to_string(id));
    }

    int findClassIndexById(const string& id) const
    {
        for (int i = 0; i < (int)classes.size(); i++)
        {
            if (classes[i].id == id)
                return i;
        }
        return -1;
    }

    bool teacherHasAssignedClass(int teacherId) const
    {
        for (const auto& c : classes)
        {
            if (c.coVanHocTapId == teacherId)
                return true;
        }
        return false;
    }

    void saveKetQua() const
    {
        ofstream fout("ketqua.txt");
        for (const auto& hs : students)
        {
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                fout << kq.idSV << '|' << kq.tenSV << '|' << kq.mon << '|' << kq.diem << '|' << kq.thoiGian << '|' << kq.maDe << '|' << kq.loaiThi << '|' << (kq.cameraOn ? 1 : 0) << '|' << kq.cameraNote << '\n';
            }
        }
        fout.close();
    }

    void loadKetQua()
    {
        ifstream fin("ketqua.txt");
        if (!fin) return;
        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            stringstream ss(line);
            vector<string> tk; string t;
            while (getline(ss, t, '|')) tk.push_back(t);
            if (tk.size() < 7) continue;

            KetQua kq;
            kq.idSV = safeStoi(tk[0]);
            kq.tenSV = tk[1];
            kq.mon = tk[2];
            kq.diem = safeStod(tk[3]);
            kq.thoiGian = safeStoi(tk[4]);
            kq.maDe = safeStoi(tk[5]);
            kq.loaiThi = safeStoi(tk[6]);
            if (tk.size() > 7)
                kq.cameraOn = safeStoi(tk[7]) == 1;
            else
                kq.cameraOn = false;
            if (tk.size() > 8)
                kq.cameraNote = tk[8];
            else
                kq.cameraNote = "";

            for (auto& hs : students)
            {
                if (hs.getId() == kq.idSV)
                {
                    vector<KetQua> ds = hs.getDanhSachKetQua();
                    ds.push_back(kq);
                    hs.setDanhSachKetQua(ds);
                    break;
                }
            }
        }
        fin.close();
    }

    void recalcTotalStudents()
    {
        totalStudents = 0;
        for (const auto& c : classes)
        {
            totalStudents += c.studentCount;
        }
    }

    // ==================== LOGIN ====================
    bool loginAdmin()
    {
        SetColor(MAGENTA);

        cout << "╔════════════════════════════════════╗\n";
        cout << "║         DANG NHAP ADMIN            ║\n";
        cout << "╚════════════════════════════════════╝\n";

        string user, pass;

        SetColor(CYAN);
        cout << "Username: ";
        SetColor(WHITE);
        getline(cin, user);

        SetColor(CYAN);
        cout << "Password: ";
        SetColor(WHITE);
        getline(cin, pass);

        if (adminAccount.login(user, pass))
        {
            SetColor(GREEN);
            cout << "\nDang nhap thanh cong.\n";

            SetColor(WHITE);
            return true;
        }

        SetColor(RED);
        cout << "\nSai username hoac password.\n";

        SetColor(WHITE);
        return false;
    }

    //==================== STUDENT CRUD =====================
    void addStudent()
    {
        cout << "\n===== THEM SINH VIEN =====\n";
        SinhVien hs;
        hs.setId(nhapSo<int>("Nhap ID sinh vien: "));

        if (studentIdExists(hs.getId()))
        {
            cout << "ID sinh vien da ton tai.\n";
            return;
        }

        hs.setUsername(inputLine("Nhap username: "));
        if (!isValidUsername(hs.getUsername()))
        {
            cout << "Username khong hop le._.\n";
            return;
        }
        if (usernameExists(hs.getUsername()))
        {
            cout << "Username da ton tai.\n";
            return;
        }

        hs.setPassword(inputLine("Nhap password: "));
        if (!isValidPassword(hs.getPassword()))
        {
            cout << "Password khong hop le. Toi thieu 6 ky tu.\n";
            return;
        }

        hs.setFullName(inputLine("Nhap ho ten: "));
        if (!isValidName(hs.getFullName()))
        {
            cout << "Ho ten khong hop le.\n";
            return;
        }

        string tenLop;
        bool hopLe = false;

        cout << "\n===== DANH SACH LOP =====\n";
        for (const auto& c : classes)
        {
            cout << "- " << c.className << endl;
        }

        do
        {
            tenLop = inputLine("Nhap lop: ");
            hopLe = false;

            for (const auto& c : classes)
            {
                if (c.className == tenLop)
                {
                    hopLe = true;
                    break;
                }
            }
        

            if (!hopLe)
            {
                cout << "Lop khong ton tai! Vui long nhap lai.\n";
            }

        } while (!hopLe);

        hs.setlopChuNhiem(tenLop);
        int gt;

        do
        {
            gt = nhapSo<int>("Nhap gioi tinh (1-Nam, 2-Nu, 3-Khac): ");

            if (gt < 1 || gt > 3)
            {
                cout << "Chi duoc nhap 1, 2 hoac 3!\n";
            }

        }   
        while (gt < 1 || gt > 3);
        string gender;

        switch (gt)
        {
        case 1:
            gender = "Nam";
            break;

        case 2:
            gender = "Nu";
            break;

        case 3:
            gender = "Khac";
            break;
        }

        hs.setGioiTinh(gender);
        string ngaySinh;

        do
        {
            ngaySinh = inputLine("Nhap ngay sinh (dd/mm/yyyy): ");

            if (!isValidDate(ngaySinh))
            {
                cout << "Ngay sinh khong hop le! Vui long nhap dung dinh dang dd/mm/yyyy.\n";
            }

        } while (!isValidDate(ngaySinh));

        hs.setBirthDay(ngaySinh);
        string sdt;

        do
        {
            sdt = inputLine("Nhap so dien thoai (10 so): ");

            if (!isValidPhone(sdt))
            {
                cout << "So dien thoai khong hop le! Phai gom dung 10 chu so.\n";
            }

        } while (!isValidPhone(sdt));

        hs.setSdt(sdt);
        double diemTB;

        do
        {
            diemTB = nhapSo<double>("Nhap diem trung binh (0 - 10): ");

            if (diemTB < 0 || diemTB > 10)
            {
                cout << "Diem khong hop le! Chi duoc nhap tu 0 den 10.\n";
            }

        } while (diemTB < 0 || diemTB > 10);

        // Lam tron 2 chu so thap phan
        diemTB = round(diemTB * 100) / 100;

        hs.setDiemTB(diemTB);
        int teacherId = nhapSo<int>("Nhap ID giang vien phu trach: ");
        if (!teacherIdExists(teacherId))
        {
            cout << "ID giang vien khong ton tai.\n";
            return;
        }

        hs.setTeacherId(teacherId);
        students.push_back(hs);
        cout << "Them sinh vien thanh cong.\n";
    }

    void viewStudents() const
    {
        cout << "\n===== DANH SACH SINH VIEN =====\n";
        if (students.empty())
        {
            cout << "Chua co sinh vien nao.\n";
            return;
        }
        for (int i = 0; i < (int)students.size(); i++)
        {
            cout << "STT: " << i + 1 << " | ID: " << students[i].getId() << " | Username: " << students[i].getUsername() << " | Full name: " << students[i].getFullName() << '\n';
        }
    }

    void editStudent()
    {
        cout << "\n===== SUA SINH VIEN =====\n";
        int id = nhapSo<int>("Nhap ID sinh vien can sua: ");
        int idx = findStudentIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay sinh vien.\n";
            return;
        }

        cout << "1. Sua username\n2. Sua password\n3. Sua ho ten\n4. Sua lop\n5. Sua gioi tinh\n6. Sua ngay sinh\n7. Sua so dien thoai\n8. Sua diem trung binh\n9. Sua giang vien phu trach\n10. Sua tat ca\n";
        int chHS = nhapSo<int>("Chon: ");

        if (chHS == 1 || chHS == 10)
        {
            string newUsername = inputLine("Nhap username moi: ");
            if (!isValidUsername(newUsername))
            {
                cout << "Username khong hop le.\n";
                return;
            }
            if (newUsername != students[idx].getUsername() && usernameExists(newUsername))
            {
                cout << "Username da ton tai.\n";
                return;
            }
            students[idx].setUsername(newUsername);
        }
        if (chHS == 2 || chHS == 10)
        {
            string newPassword = inputLine("Nhap password moi: ");
            if (!isValidPassword(newPassword))
            {
                cout << "Password khong hop le.\n";
                return;
            }
            students[idx].setPassword(newPassword);
        }
        if (chHS == 3 || chHS == 10)
        {
            string newFullName = inputLine("Nhap ho ten moi: ");
            if (!isValidName(newFullName))
            {
                cout << "Ho ten khong hop le.\n";
                return;
            }
            students[idx].setFullName(newFullName);
        }
        if (chHS == 4 || chHS == 10)
        {
            string newClass = inputLine("Nhap lop moi: ");
            students[idx].setlopChuNhiem(newClass);
        }
        if (chHS == 5 || chHS == 10)
        {
            string newGender = inputLine("Nhap gioi tinh moi: ");
            students[idx].setGioiTinh(newGender);
        }
        if (chHS == 6 || chHS == 10)
        {
            string newBirthDay = inputLine("Nhap ngay sinh moi: ");
            students[idx].setBirthDay(newBirthDay);
        }
        if (chHS == 7 || chHS == 10)
        {
            string newPhone = inputLine("Nhap so dien thoai moi: ");
            students[idx].setSdt(newPhone);
        }
        if (chHS == 8 || chHS == 10)
        {
            double newDiemTB = nhapSo<double>("Nhap diem trung binh moi: ");
            students[idx].setDiemTB(newDiemTB);
        }
        if (chHS == 9 || chHS == 10)
        {
            int newTeacherId = nhapSo<int>("Nhap ID giang vien moi: ");
            if (!teacherIdExists(newTeacherId))
            {
                cout << "ID giang vien khong ton tai.\n";
                return;
            }
            students[idx].setTeacherId(newTeacherId);
        }
        cout << "Sua sinh vien thanh cong.\n";
    }

    void deleteStudent()
    {
        cout << "\n===== XOA SINH VIEN =====\n";
        int id = nhapSo<int>("Nhap ID sinh vien can xoa: ");
        int idx = findStudentIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay sinh vien.\n";
            return;
        }
        students.erase(students.begin() + idx);
        cout << "Xoa sinh vien thanh cong.\n";
    }

    vector<SinhVien>& getStudents() { return students; }

    // ==================== TEACHER CRUD ====================
    void addTeacher()
    {
        cout << "\n===== THEM GIANG VIEN =====\n";
        GiangVien t;
        t.setId(nhapSo<int>("Nhap ID giang vien: "));
        if (teacherIdExists(t.getId()))
        {
            cout << "ID giang vien da ton tai.\n";
            return;
        }

        t.setUsername(inputLine("Nhap username: "));
        if (!isValidUsername(t.getUsername()))
        {
            cout << "Username khong hop le.\n";
            return;
        }
        if (usernameExists(t.getUsername()))
        {
            cout << "Username da ton tai.\n";
            return;
        }

        t.setPassword(inputLine("Nhap password: "));
        if (!isValidPassword(t.getPassword()))
        {
            cout << "Password phai >= 6 ky tu.\n";
            return;
        }

        t.setFullName(inputLine("Nhap ho ten: "));
        string mon = inputLine("Nhap mon day: ");
        t.setMon(mon);

        cout << "\n=== Chon vai tro ===\n";
        cout << "1. Giang vien bo mon\n";
        cout << "2. Giang vien bo mon kiem co van\n";
        
        int roleChoice;
        do
        {
            roleChoice = nhapSo<int>("Chon (1-2): ");
            if (roleChoice < 1 || roleChoice > 2)
            {
                cout << "Lua chon khong hop le.\n";
            }
        } while (roleChoice < 1 || roleChoice > 2);

        string roles = (roleChoice == 2 ? "BoMon,CoVan" : "BoMon");
        t.setTeacherRole(roles);
        teachers.push_back(t);
        cout << "Them giang vien thanh cong.\n";
    }

    void viewTeachers() const
    {
        cout << "\n===== DANH SACH GIANG VIEN =====\n";
        if (teachers.empty())
        {
            cout << "Chua co giang vien nao.\n";
            return;
        }
        for (int i = 0; i < (int)teachers.size(); i++)
        {
            cout << "STT: " << i + 1
                << " | ID: " << teachers[i].getId()
                << " | Username: " << teachers[i].getUsername()
                << " | Full Name: " << teachers[i].getFullName()
                << " | Mon: " << teachers[i].getMon()
                << " | Vai tro: " << teachers[i].getTeacherRoleLabel();
            if (!teachers[i].getAdvisorClass().empty())
                cout << " | Lop chu nhiem: " << teachers[i].getAdvisorClass();
            cout << '\n';
        }
    }

    void editTeacher()
    {
        cout << "\n===== SUA GIANG VIEN =====\n";
        int id = nhapSo<int>("Nhap ID giang vien can sua: ");
        int idx = findTeacherIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay giang vien.\n";
            return;
        }

        cout << "1. Sua username\n2. Sua password\n3. Sua ho ten\n4. Sua vai tro\n5. Sua tat ca\n";
        int choice = nhapSo<int>("Chon: ");

        if (choice == 1 || choice == 5)
        {
            string newUsername = inputLine("Nhap username moi: ");
            if (!isValidUsername(newUsername))
            {
                cout << "Username khong hop le.\n";
                return;
            }
            if (newUsername != teachers[idx].getUsername() && usernameExists(newUsername))
            {
                cout << "Username da ton tai.\n";
                return;
            }
            teachers[idx].setUsername(newUsername);
        }

        if (choice == 2 || choice == 5)
        {
            string newPassword = inputLine("Nhap password moi: ");
            if (!isValidPassword(newPassword))
            {
                cout << "Password khong hop le.\n";
                return;
            }
            teachers[idx].setPassword(newPassword);
        }

        if (choice == 3 || choice == 5)
        {
            string newFullName = inputLine("Nhap ho ten moi: ");
            if (!isValidName(newFullName))
            {
                cout << "Ho ten khong hop le.\n";
                return;
            }
            teachers[idx].setFullName(newFullName);
        }

        if (choice == 4 || choice == 5)
        {
            cout << "\n=== Chon vai tro moi ===\n";
            cout << "1. Giang vien bo mon\n";
            cout << "2. Giang vien bo mon kiem co van\n";
            
            int roleChoice;
            do
            {
                roleChoice = nhapSo<int>("Chon (1-2): ");
                if (roleChoice < 1 || roleChoice > 2)
                {
                    cout << "Lua chon khong hop le.\n";
                }
            } while (roleChoice < 1 || roleChoice > 2);

            string roles = (roleChoice == 2 ? "BoMon,CoVan" : "BoMon");
            
            // If removing CoVan role but teacher currently is advisor, prevent it
            int thisTeacherId = teachers[idx].getId();
            bool willBeAdvisor = (roles.find("CoVan") != string::npos);
            bool currentlyAdvisor = teacherHasAssignedClass(thisTeacherId);
            if (!willBeAdvisor && currentlyAdvisor)
            {
                cout << "Khong the bo vai tro CoVan. Giang vien dang la co van cua mot lop.\n";
                return;
            }

            // If will be advisor but currently has no class, require assignment
            if (willBeAdvisor && !currentlyAdvisor)
            {
                // find available classes without advisor
                vector<int> freeIdx;
                for (int i = 0; i < (int)classes.size(); i++)
                {
                    if (classes[i].coVanHocTapId == 0)
                        freeIdx.push_back(i);
                }
                if (freeIdx.empty())
                {
                    cout << "Khong co lop trong de gan cho CoVan. Tao lop truoc khi gan vai tro.\n";
                    return;
                }
                cout << "Chon lop de giao lam CoVan: \n";
                for (size_t i = 0; i < freeIdx.size(); i++)
                {
                    cout << i+1 << ". " << classes[freeIdx[i]].className << "\n";
                }
                int pick = nhapSo<int>("Chon (so): ");
                if (pick < 1 || pick > (int)freeIdx.size())
                {
                    cout << "Lua chon khong hop le.\n";
                    return;
                }
                int classIdx = freeIdx[pick-1];
                classes[classIdx].coVanHocTapId = thisTeacherId;
                for (int sid : classes[classIdx].students)
                {
                    int sidx = findStudentIndexById(sid);
                    if (sidx != -1) students[sidx].setTeacherId(thisTeacherId);
                }
            }

            teachers[idx].setTeacherRole(roles);
        }

        cout << "Sua giang vien thanh cong.\n";
    }

    void deleteTeacher()
    {
        cout << "\n===== XOA GIANG VIEN =====\n";
        int id = nhapSo<int>("Nhap ID giang vien can xoa: ");
        int idx = findTeacherIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay giang vien.\n";
            return;
        }
        if (teacherHasAssignedClass(id))
        {
            cout << "Khong the xoa. giang vien dang duoc gan cho lop.\n";
            return;
        }

        teachers.erase(teachers.begin() + idx);
        cout << "Xoa giang vien thanh cong.\n";
    }

    vector<GiangVien>& getTeachers() { return teachers; }

    // ==================== CLASS CRUD ====================
    bool teacherIsAdvisorOfAnyClass(int teacherId) const {
        for (const auto& c : classes) {
            if (c.coVanHocTapId == teacherId) return true;
        }
        return false;
    }

    string getAdvisorClassName(int teacherId) const
    {
        for (const auto& c : classes)
        {
            if (c.coVanHocTapId == teacherId)
                return c.className;
        }

        return "";
    }

    void addClass()
    {
        int choice;

        do
        {
            cout << "\n===== THEM LOP =====\n";
            cout << "1. Them lop moi\n";
            cout << "2. Quan ly sinh vien\n";
            cout << "0. Quay lai\n";

            choice = nhapSo<int>("Chon: ");

            switch (choice)
            {
                case 1:
                {
                    ClassInfo c;

                    while (true)
                    {
                        c.id = inputLine("Nhap ID lop: ");

                        if (c.id.empty())
                        {
                            cout << "ID lop khong duoc rong.\n";
                            continue;
                        }

                        bool hopLe = true;

                        for (char ch : c.id)
                        {
                            if (!isalnum(ch))
                            {
                                hopLe = false;
                                break;
                            }
                        }

                        if (!hopLe)
                        {  
                            cout << "ID lop chi duoc la so. Khong duoc co ky tu dac biet.\n";
                            continue;
                        }

                        if (classIdExists(c.id))
                        {
                            cout << "Lop da ton tai.\n";
                            continue;
                        }

                        break;
                }

                    if (classIdExists(c.id))
                    {
                        cout << "Them lop khong thanh cong.\n";
                        break;
                        return;
                    }

                    c.className = inputLine("Nhap ten lop: ");

                    if (c.className.empty())
                {
                    cout << "Them lop khong thanh cong: Ten lop khong duoc rong.\n";
                    break;
                    return;
                }

                c.studentCount = nhapSo<int>("Nhap so luong sinh vien: ");

                if (c.studentCount < 0)
                {
                    cout << "Them lop khong thanh cong: So luong sinh vien khong hop le.\n";
                    break;
                    return;
                }

                c.students.clear();

                for (int i = 0; i < c.studentCount; i++)
                {
                    while (true)
                    {
                        int idSV = nhapSo<int>(
                        "Nhap ID sinh vien thu " + to_string(i + 1) + ": ");

                        int idxSV = findStudentIndexById(idSV);

                        if (idxSV == -1)
                        {
                            SetColor(YELLOW);
                            cout << "ID sinh vien khong ton tai.";
                            SetColor(WHITE);
                            string createNew = inputLine(" Ban co muon tao sinh vien moi voi ID nay? (y/n): ");
                            if (createNew.empty() || (createNew[0] != 'y' && createNew[0] != 'Y'))
                            {
                                cout << "Bo qua. Nhap lai ID.\n";
                                continue;
                            }

                            // Create new student interactively
                            SinhVien sv;
                            sv.setId(idSV);

                            // Username
                            while (true)
                            {
                                string uname = inputLine("Nhap username cho sinh vien: ");
                                if (!isValidUsername(uname))
                                {
                                    cout << "Username khong hop le. Chi chua chu cai. Thu lai.\n";
                                    continue;
                                }
                                if (usernameExists(uname))
                                {
                                    cout << "Username da ton tai. Thu lai.\n";
                                    continue;
                                }
                                sv.setUsername(uname);
                                break;
                            }

                            // Password
                            while (true)
                            {
                                string pw = inputLine("Nhap mat khau (toi thieu 6 ky tu): ");
                                if (!isValidPassword(pw))
                                {
                                    cout << "Mat khau khong hop le. Thu lai.\n";
                                    continue;
                                }
                                sv.setPassword(pw);
                                break;
                            }

                            // Full name
                            while (true)
                            {
                                string ten = inputLine("Nhap ho ten: ");
                                if (!isValidName(ten))
                                {
                                    cout << "Ho ten khong hop le. Thu lai.\n";
                                    continue;
                                }
                                sv.setFullName(ten);
                                break;
                            }

                            // Gender (optional)
                            int gt = nhapSo<int>("Nhap gioi tinh (1-Nam,2-Nu,3-Khac): ");
                            if (gt == 1) sv.setGioiTinh("Nam");
                            else if (gt == 2) sv.setGioiTinh("Nu");
                            else sv.setGioiTinh("Khac");

                            sv.setlopChuNhiem(c.className);
                            sv.setTeacherId(0);
                            students.push_back(sv);
                            idxSV = (int)students.size() - 1;
                            cout << "Tao sinh vien moi thanh cong: " << sv.getFullName() << " (ID " << sv.getId() << ")\n";
                        }

                        bool daCoLop = false;

                        for (const auto& lop : classes)
                        {
                            for (int id : lop.students)
                            {
                                if (id == idSV)
                                {
                                    daCoLop = true;
                                    break;
                                }
                            }

                            if (daCoLop)
                                break;
                        }

                        if (daCoLop)
                        {
                            SetColor(YELLOW);
                            cout << "Sinh vien da co lop. Thu lai.\n";
                            SetColor(WHITE);
                            continue;
                        }

                        c.students.push_back(idSV);
                        break;
                    }
                }

                int advisorId;
                do
                {
                    advisorId = nhapSo<int>("Nhap ID Co van hoc tap cho lop (0 neu chua gan): ");
                    if (advisorId == 0) break;
                    if (!teacherIdExists(advisorId))
                    {
                        cout << "ID giang vien khong ton tai. Vui long nhap lai.\n";
                        continue;
                    }
                    // Enforce single advisor: teacher cannot be advisor of another class
                    if (teacherHasAssignedClass(advisorId))
                    {
                        cout << "Giang vien nay dang lam co van cua lop khac. Chon giang vien khac.\n";
                        continue;
                    }
                    break;
                } while (true);

                c.coVanHocTapId = advisorId;
                    if (c.coVanHocTapId != 0)
                {
                    int advisorIdx = findTeacherIndexById(c.coVanHocTapId);
                    if (advisorIdx != -1 && !teachers[advisorIdx].hasRole("CoVan"))
                    {
                        teachers[advisorIdx].setTeacherRole("BoMon,CoVan");
                    }
                    for (int idSV : c.students)
                    {
                        int idxSV = findStudentIndexById(idSV);
                        if (idxSV != -1)
                            students[idxSV].setTeacherId(c.coVanHocTapId);
                    }
                }

                string addMore;
                do
                {
                    int teacherId = nhapSo<int>("Nhap ID giang vien bo mon cho lop (0 de dung): ");
                    if (teacherId == 0)
                        break;
                    if (!teacherIdExists(teacherId))
                    {
                        cout << "ID giang vien khong ton tai.\n";
                        continue;
                    }
                    if (!isSubjectTeacherId(teacherId))
                    {
                        cout << "Chi giang vien bo mon moi duoc them.\n";
                        continue;
                    }
                    if (find(c.danhSachGiangVien.begin(), c.danhSachGiangVien.end(), teacherId) != c.danhSachGiangVien.end())
                    {
                        cout << "Giang vien nay da duoc them cho lop.\n";
                        continue;
                    }
                    c.danhSachGiangVien.push_back(teacherId);
                    addMore = inputLine("Tiep tuc them giang vien bo mon? (y/n): ");
                } while (!addMore.empty() && (addMore[0] == 'y' || addMore[0] == 'Y'));

                classes.push_back(c);

                SetColor(GREEN);
                cout << "Them lop thanh cong.\n";
                SetColor(WHITE);

                break;
            }

            case 2:
            {
                int svChoice;

                do
                {
                    cout << "\n===== QUAN LY SINH VIEN =====\n";
                    cout << "1. Them sinh vien\n";
                    cout << "2. Sua sinh vien\n";
                    cout << "3. Xoa sinh vien\n";
                    cout << "0. Quay lai\n";

                    svChoice = nhapSo<int>("Chon: ");

                    switch (svChoice)
                    {
                    case 1:
                        addStudent();
                        break;

                    case 2:
                        editStudent();
                        break;

                    case 3:
                        deleteStudent();
                        break;

                    case 0:
                        break;

                    default:
                        cout << "Lua chon khong hop le.\n";
                    }

                } while (svChoice != 0);

                break;
            }

            case 0:
                break;

            default:
                cout << "Lua chon khong hop le.\n";
            }

        } while (choice != 0);
    }
    void quanLySinhVienTrongLop()
    {
        int choice;

        do
        {
            cout << "\n===== QUAN LY SINH VIEN =====\n";
            cout << "1. Them sinh vien\n";
            cout << "2. Sua sinh vien\n";
            cout << "3. Xoa sinh vien\n";
            cout << "0. Quay lai\n";

            choice = nhapSo<int>("Chon: ");

            switch (choice)
            {
            case 1:
                addStudent();
                break;

            case 2:
                editStudent();
                break;

            case 3:
                deleteStudent();
                break;

            case 0:
                break;

            default:
                cout << "Lua chon khong hop le.\n";
            }

        } while (choice != 0);
    }
    void viewClasses() const
    {
        cout << "\n===== DANH SACH LOP =====\n";
        if (classes.empty())
        {
            cout << "Chua co lop nao.\n";
            return;
        }
        for (int i = 0; i < (int)classes.size(); i++)
        {
            const auto& c = classes[i];
            cout << "STT: " << i + 1 << " | ID lop: " << c.id
                << " | Ten lop: " << c.className
                << " | CVHT ID: " << c.coVanHocTapId
                << " (" << getTeacherNameById(c.coVanHocTapId) << ")"
                << " | So GV day: " << c.danhSachGiangVien.size()
                << " | So SV: " << c.studentCount << '\n';
            
            if (!c.danhSachGiangVien.empty())
            {
                cout << "    Danh sach giang vien bo mon: ";
                for (int j = 0; j < (int)c.danhSachGiangVien.size(); j++)
                {
                    int gvId = c.danhSachGiangVien[j];
                    cout << gvId << "(" << getTeacherNameById(gvId) << ")";
                    if (j + 1 < (int)c.danhSachGiangVien.size())
                        cout << ", ";
                }
                cout << '\n';
            }
            else
            {
                cout << "    Danh sach giang vien bo mon: (Chua co)\n";
            }
            
            // Hien thi danh sach sinh vien
            cout << "    Danh sach sinh vien: ";
            int countSV = 0;
            for (const auto& hs : students)
            {
                if (hs.getLop() == c.className)
                {
                    if (countSV > 0) cout << ", ";
                    cout << hs.getId() << "(" << hs.getFullName() << ")";
                    countSV++;
                }
            }
            if (countSV == 0)
                cout << "(Chua co)";
            cout << '\n';
        }
    }

    void viewClassesFromFile() const
    {
        cout << "\n===== DANH SACH LOP TRONG FILE lophoc.txt =====\n";
        ifstream fin("lophoc.txt");
        if (!fin)
        {
            cout << "Khong tim thay file lophoc.txt.\n";
            return;
        }
        string line;
        int idx = 1;
        while (getline(fin, line))
        {
            if (line.empty())
                continue;
            if (line.back() == '\r') line.pop_back();
            stringstream ss(line);
            vector<string> tk;
            string token;
            while (getline(ss, token, '|'))
                tk.push_back(token);
            if (tk.size() >= 4)
            {
                cout << idx++ << ". ID lop: " << tk[1]
                    << " | Ten lop: " << tk[2]
                    << " | CVHT ID: " << tk[3]
                    << " | So SV: " << (tk.size() > 4 ? tk[4] : "0") << '\n';
            }
            else
            {
                cout << idx++ << ". " << line << '\n';
            }
        }
        fin.close();
    }

    void editClass()
    {
        cout << "\n===== SUA LOP =====\n";
        string id = inputLine("Nhap ID lop can sua: ");
        int idx = findClassIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay lop.\n";
            return;
        }

        cout << "1. Sua ten lop\n2. Sua giang vien phu trach\n3. Sua so luong sinh vien\n4. Sua tat ca\n5. Quan ly giang vien bo mon cho lop\n";
        int choice = nhapSo<int>("Chon: ");

        if (choice == 1 || choice == 4)
        {
            string newClassName = inputLine("Nhap ten lop moi: ");
            classes[idx].className = newClassName;
        }

        if (choice == 2 || choice == 4)
        {
            int newTeacherId = nhapSo<int>("Nhap ID giang vien moi (0 neu bo gan): ");
            if (newTeacherId != 0 && !teacherIdExists(newTeacherId))
            {
                cout << "giang vien khong ton tai.\n";
                return;
            }
            // Prevent assigning a teacher who already is advisor of another class
            if (newTeacherId != 0 && newTeacherId != classes[idx].coVanHocTapId && teacherHasAssignedClass(newTeacherId))
            {
                cout << "Giang vien nay dang lam co van cua lop khac. Khong the gan.\n";
                return;
            }
            int oldAdvisorId = classes[idx].coVanHocTapId;
            classes[idx].coVanHocTapId = newTeacherId;
            if (oldAdvisorId != 0 && oldAdvisorId != newTeacherId)
            {
                int oldIdx = findTeacherIndexById(oldAdvisorId);
                if (oldIdx != -1)
                    teachers[oldIdx].setTeacherRole("BoMon");
            }
            if (newTeacherId != 0)
            {
                int newIdx = findTeacherIndexById(newTeacherId);
                if (newIdx != -1)
                    teachers[newIdx].setTeacherRole("BoMon,CoVan");
                for (int idSV : classes[idx].students)
                {
                    int idxSV = findStudentIndexById(idSV);
                    if (idxSV != -1)
                        students[idxSV].setTeacherId(newTeacherId);
                }
            }
        }

        if (choice == 3 || choice == 4)
        {
            int newStudentCount = nhapSo<int>("Nhap so luong sinh vien moi: ");
            if (newStudentCount < 0)
            {
                cout << "So luong sinh vien khong hop le.\n";
                return;
            }
            classes[idx].studentCount = newStudentCount;
            recalcTotalStudents();
        }

        if (choice == 5 || choice == 4)
        {
            manageClassTeachers(classes[idx]);
        }

        saveData();

        cout << "Sua lop thanh cong.\n";
    }

    void manageClassTeachers(ClassInfo& c)
    {
        int choice;
        do
        {
            cout << "\n===== QUAN LY GIANG VIEN BO MON CHO LOP " << c.className << " =====\n";
            cout << "Co van hoc tap: " << c.coVanHocTapId << " (" << getTeacherNameById(c.coVanHocTapId) << ")\n";
            cout << "Giang vien bo mon hien co: ";
            if (c.danhSachGiangVien.empty())
            {
                cout << "(Chua co)\n";
            }
            else
            {
                for (int i = 0; i < (int)c.danhSachGiangVien.size(); i++)
                {
                    int gvId = c.danhSachGiangVien[i];
                    cout << gvId << "(" << getTeacherNameById(gvId) << ")";
                    if (i + 1 < (int)c.danhSachGiangVien.size())
                        cout << ", ";
                }
                cout << "\n";
            }

            cout << "1. Gan/Cai dat Co van hoc tap\n";
            cout << "2. Them giang vien bo mon\n";
            cout << "3. Xoa giang vien bo mon\n";
            cout << "0. Quay lai\n";
            choice = nhapSo<int>("Chon: ");

            if (choice == 1)
            {
                int newAdvisorId = nhapSo<int>("Nhap ID Co van hoc tap moi (0 neu bo gan): ");
                if (newAdvisorId != 0 && !teacherIdExists(newAdvisorId))
                {
                    cout << "ID giang vien khong ton tai.\n";
                    continue;
                }
                // Prevent assigning a teacher who is already advisor of another class
                if (newAdvisorId != 0 && newAdvisorId != c.coVanHocTapId && teacherHasAssignedClass(newAdvisorId))
                {
                    cout << "Giang vien nay da la co van cua lop khac. Chon giang vien khac.\n";
                    continue;
                }
                int oldAdvisorId = c.coVanHocTapId;
                c.coVanHocTapId = newAdvisorId;
                if (oldAdvisorId != 0 && oldAdvisorId != newAdvisorId)
                {
                    int oldIdx = findTeacherIndexById(oldAdvisorId);
                    if (oldIdx != -1)
                        teachers[oldIdx].setTeacherRole("BoMon");
                }
                if (newAdvisorId != 0)
                {
                    int newIdx = findTeacherIndexById(newAdvisorId);
                    if (newIdx != -1)
                        teachers[newIdx].setTeacherRole("BoMon,CoVan");
                    for (int studentId : c.students)
                    {
                        int studentIdx = findStudentIndexById(studentId);
                        if (studentIdx != -1)
                            students[studentIdx].setTeacherId(newAdvisorId);
                    }
                }
            }
            else if (choice == 2)
            {
                int teacherId = nhapSo<int>("Nhap ID giang vien bo mon can them: ");
                if (!teacherIdExists(teacherId))
                {
                    cout << "ID giang vien khong ton tai.\n";
                    continue;
                }
                if (!isSubjectTeacherId(teacherId))
                {
                    cout << "Chi giang vien bo mon moi duoc them.\n";
                    continue;
                }
                if (find(c.danhSachGiangVien.begin(), c.danhSachGiangVien.end(), teacherId) != c.danhSachGiangVien.end())
                {
                    cout << "Giang vien nay da duoc them cho lop.\n";
                    continue;
                }
                c.danhSachGiangVien.push_back(teacherId);
            }
            else if (choice == 3)
            {
                int teacherId = nhapSo<int>("Nhap ID giang vien bo mon can xoa: ");
                auto it = find(c.danhSachGiangVien.begin(), c.danhSachGiangVien.end(), teacherId);
                if (it == c.danhSachGiangVien.end())
                {
                    cout << "Giang vien nay khong thuoc lop.\n";
                    continue;
                }
                c.danhSachGiangVien.erase(it);
            }
            else if (choice != 0)
            {
                cout << "Lua chon khong hop le.\n";
            }

        } while (choice != 0);
    }

    void deleteClass()
    {
        cout << "\n===== XOA LOP =====\n";
        int id = nhapSo<int>("Nhap ID lop can xoa: ");
        int idx = findClassIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay lop.\n";
            return;
        }
        if (classes[idx].coVanHocTapId != 0)
        {
            int advisorIdx = findTeacherIndexById(classes[idx].coVanHocTapId);
            if (advisorIdx != -1)
                teachers[advisorIdx].setTeacherRole("BoMon");
        }

        classes.erase(classes.begin() + idx);
        recalcTotalStudents();
        cout << "Xoa lop thanh cong.\n";
    }

    // ==================== STATISTICS ====================

    bool roomIdExists(int id) const
    {
        for (const auto& r : examRooms)
        {
            if (r.id == id)
                return true;
        }
        return false;
    }

    int findRoomIndexById(int id) const
    {
        for (int i = 0; i < (int)examRooms.size(); i++)
        {
            if (examRooms[i].id == id)
                return i;
        }
        return -1;
    }

    void addRoom()
    {
        cout << "\n===== THEM PHONG THI =====\n";
        PhongThi r;
        r.id = nhapSo<int>("Nhap ID phong thi: ");
        if (roomIdExists(r.id))
        {
            cout << "ID phong thi da ton tai.\n";
            return;
        }
        r.tenPhong = inputLine("Nhap ten phong: ");
        r.sucChua = nhapSo<int>("Nhap suc chua: ");
        r.locked = false;
        examRooms.push_back(r);
        cout << "Them phong thi thanh cong.\n";
    }

    void viewRooms() const
    {
        cout << "\n===== DANH SACH PHONG THI =====\n";
        if (examRooms.empty())
        {
            cout << "Chua co phong thi nao.\n";
            return;
        }
        for (int i = 0; i < (int)examRooms.size(); i++)
        {
            cout << "STT: " << i + 1 << " | ID: " << examRooms[i].id << " | Ten phong: " << examRooms[i].tenPhong
                << " | Suc chua: " << examRooms[i].sucChua
                << " | Trang thai: " << (examRooms[i].locked ? "Khoa" : "Mo") << '\n';
        }
    }

    void editRoom()
    {
        cout << "\n===== SUA PHONG THI =====\n";
        int id = nhapSo<int>("Nhap ID phong thi can sua: ");
        int idx = findRoomIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay phong thi.\n";
            return;
        }
        cout << "1. Sua ten phong\n2. Sua suc chua\n3. Khoa/Mo phong\n4. Sua tat ca\n";
        int choice = nhapSo<int>("Chon: ");
        if (choice == 1 || choice == 4)
        {
            examRooms[idx].tenPhong = inputLine("Nhap ten phong moi: ");
        }
        if (choice == 2 || choice == 4)
        {
            examRooms[idx].sucChua = nhapSo<int>("Nhap suc chua moi: ");
        }
        if (choice == 3 || choice == 4)
        {
            examRooms[idx].locked = !examRooms[idx].locked;
            cout << "Phong thi da " << (examRooms[idx].locked ? "duoc khoa" : "duoc mo") << ".\n";
        }
        cout << "Sua phong thi thanh cong.\n";
    }

    void deleteRoom(QuanLyThi& thi)
    {
        cout << "\n===== XOA PHONG THI =====\n";
        int id = nhapSo<int>("Nhap ID phong thi can xoa: ");
        int idx = findRoomIndexById(id);
        if (idx == -1)
        {
            cout << "Khong tim thay phong thi.\n";
            return;
        }
        thi.clearRoomAssignments(id);
        examRooms.erase(examRooms.begin() + idx);
        thi.saveDeThi();
        cout << "Xoa phong thi thanh cong.\n";
    }

    void manageExamRooms(QuanLyThi& thi)
    {
        int choice;
        do
        {
            cout << "\n===== QUAN LY PHONG THI =====\n";
            cout << "1. Them phong thi\n2. Sua phong thi\n3. Xoa phong thi\n4. Xem phong thi\n0. Quay lai\n";
            choice = nhapSo<int>("Chon: ");
            switch (choice)
            {
            case 1:
                addRoom();
                break;
            case 2:
                editRoom();
                break;
            case 3:
                deleteRoom(thi);
                break;
            case 4:
                viewRooms();
                break;
            }
        } while (choice != 0);
    }

    void exportExamReport(const QuanLyThi& thi) const
    {
        ofstream fout("bao_cao_thi.txt");
        fout << "ID GV|Ten GV|ID Lop|Lop|So Sinh Vien|So Cau Hoi|Thoi Gian Thi (Phut)|Ngay Gio Thi\n";

        for (const auto& de : thi.getMaDe())
        {
            if (de.loaiThi != 1) continue;
            int gvId = 0;
            string gvName = "N/A";
            for (const auto& t : teachers) {
                if (t.getMon() == de.tenMon) {
                    gvId = t.getId();
                    gvName = t.getFullName();
                    break;
                }
            }

            for (const string& tenLop : de.dsLopDuocThi)
            {
                string idLop = "";
                int soSV = 0;

                for (const auto& c : classes) {
                    if (c.className == tenLop) {
                        idLop = c.id;
                        break;
                    }
                }

                for (const auto& hs : students) {
                    if (hs.getLop() == tenLop) soSV++;
                }

                fout << gvId << "|"
                    << gvName << "|"
                    << idLop << "|"
                    << tenLop << "|"
                    << soSV << "|"
                    << de.danhSachCauHoi.size() << "|"
                    << de.thoiGianLamBai << "|"
                    << formatDateTime(de.batDau) << "\n";
            }
        }
        fout.close();
        SetColor(LIGHT_GREEN);
        cout << "Xuat bao cao thi thanh cong: bao_cao_thi.txt\n";
        SetColor(WHITE);
    }

    void exportExamReportCSV(const QuanLyThi& thi) const
    {
        ofstream fout("bao_cao_thi.csv");
        fout << "ID GV,Ten GV,ID Lop,Lop,So Sinh Vien,So Cau Hoi,Thoi Gian Thi (Phut),Ngay Gio Thi\n";

        for (const auto& de : thi.getMaDe())
        {
            if (de.loaiThi != 1) continue;
            int gvId = 0;
            string gvName = "N/A";
            for (const auto& t : teachers) {
                if (t.getMon() == de.tenMon) {
                    gvId = t.getId();
                    gvName = t.getFullName();
                    break;
                }
            }

            for (const string& tenLop : de.dsLopDuocThi)
            {
                string idLop = "";
                int soSV = 0;

                for (const auto& c : classes) {
                    if (c.className == tenLop) {
                        idLop = c.id;
                        break;
                    }
                }

                for (const auto& hs : students) {
                    if (hs.getLop() == tenLop) soSV++;
                }

                fout << gvId << ","
                    << gvName << ","
                    << idLop << ","
                    << tenLop << ","
                    << soSV << ","
                    << de.danhSachCauHoi.size() << ","
                    << de.thoiGianLamBai << ","
                    << formatDateTime(de.batDau) << "\n";
            }
        }
        fout.close();
        SetColor(LIGHT_GREEN);
        cout << "Xuat bao cao CSV thanh cong: bao_cao_thi.csv\n";
        SetColor(WHITE);
    }

    void viewExamStatistics(const QuanLyThi& thi) const
    {
        cout << "\n===== THONG KE BAO CAO THI =====\n";
        if (students.empty())
        {
            cout << "Chua co sinh vien!\n";
            return;
        }
        map<int, vector<double>> examScores;
        for (const auto& hs : students)
        {
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                examScores[kq.maDe].push_back(kq.diem);
            }
        }
        if (examScores.empty())
        {
            cout << "Chua co ket qua thi de thong ke.\n";
            return;
        }
        for (const auto& exam : examScores)
        {
            const deThi* de = thi.timDeThi(exam.first);
            if (!de)
                continue;
            const auto& scores = exam.second;
            double sum = 0;
            double maxScore = 0;
            double minScore = 10;
            for (double v : scores)
            {
                sum += v;
                maxScore = max(maxScore, v);
                minScore = min(minScore, v);
            }
            double avg = sum / scores.size();
            cout << "Ma de: " << de->id << " | Mon: " << de->tenMon << " | So luot thi: " << scores.size() 
                 << " | Diem TB: " << fixed << setprecision(2) << avg 
                 << " | Min: " << minScore << " | Max: " << maxScore << '\n';
            
            // Biểu đồ ASCII
            SetColor(LIGHT_GREEN);
            cout << "Bieu do diem: ";
            int barLength = min((int)scores.size(), 20);
            for (int i = 0; i < barLength; i++)
                cout << "#";
            cout << "\n";
            SetColor(WHITE);
        }
    }

    void saveRooms() const
    {
        ofstream fout("phongthi.txt");
        for (const auto& r : examRooms)
        {
            fout << r.id << '|' << r.tenPhong << '|' << r.sucChua << '|' << (r.locked ? 1 : 0) << '\n';
        }
    }

    void loadRooms(const string& fileName)
    {
        examRooms.clear();
        ifstream fin(fileName);
        if (!fin) return;
        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            stringstream ss(line);
            vector<string> tk; string t;
            while (getline(ss, t, '|')) tk.push_back(t);
            if (tk.size() < 4) continue;

            PhongThi r;
            r.id = safeStoi(tk[0]);
            r.tenPhong = tk[1];
            r.sucChua = safeStoi(tk[2]);
            r.locked = (tk[3] == "1");
            examRooms.push_back(r);
        }
        fin.close();
        cout << "Tai danh sach phong thi thanh cong.\n";
    }

    void viewSystemStatistics() const
    {
        cout << "\n===== THONG KE HOC SINH & GIAO VIEN =====\n";
        if (classes.empty()) {
            cout << "Chua co lop nao trong he thong.\n";
            return;
        }

        for (const auto& c : classes)
        {
            cout << "\n--- Lop: " << c.className << " ---\n";

            bool foundCVHT = false;
            for (const auto& t : teachers)
            {
                if (t.getId() == c.coVanHocTapId)
                {
                    cout << ">> Co van hoc tap: " << t.getFullName() << " (ID: " << t.getId() << ")\n";
                    foundCVHT = true;
                    break;
                }
            }
            if (!foundCVHT)
                cout << ">> Co van hoc tap: (Chua co)\n";

            bool foundDayLop = false;
            cout << ">> Giang vien day lop:\n";
            for (int gvId : c.danhSachGiangVien)
            {
                int idxT = findTeacherIndexById(gvId);
                if (idxT != -1)
                {
                    cout << "   - " << teachers[idxT].getFullName() << " (Mon: " << teachers[idxT].getMon() << ")\n";
                    foundDayLop = true;
                }
            }
            if (!foundDayLop)
                cout << "   (Chua co)\n";

            cout << ">> Danh sach sinh vien:\n";
            int countSV = 0;
            for (const auto& hs : students)
            {
                if (hs.getLop() == c.className)
                {
                    cout << "   - " << hs.getFullName() << " (ID: " << hs.getId() << ")\n";
                    countSV++;
                }
            }
            if (countSV == 0)
                cout << "   (Khong co sinh vien nao)\n";

            double sumDiem = 0;
            int countDiem = 0;
            for (const auto& hs : students)
            {
                if (hs.getLop() != c.className)
                    continue;
                for (const auto& kq : hs.getDanhSachKetQua())
                {
                    if (kq.loaiThi == 1)
                    {
                        sumDiem += kq.diem;
                        countDiem++;
                    }
                }
            }
            if (countDiem > 0)
            {
                double avg = sumDiem / countDiem;
                cout << "=> Diem trung binh lop: " << fixed << setprecision(2) << avg << "\n";
            }
            else
            {
                cout << "=> Chua co ket qua thi chinh thuc de tinh diem trung binh.\n";
            }
            cout << "=> Tong so sinh vien trong lop: " << countSV << "\n";
        }
    }

    void viewRanking()
    {
        string tenLop = inputLine("Nhap ten lop can xem bang xep hang (Hoac go ALL de xem toan truong): ");
        SetColor(CYAN);
        cout << "\n===== BANG XEP HANG (CHINH THUC) - LOP: " << tenLop << " =====\n";
        SetColor(WHITE);

        struct HSScore
        {
            string fullName;
            string className;
            double score;
        };
        vector<HSScore> bxh;

        for (const auto& hs : students)
        {
            if (tenLop != "ALL" && hs.getLop() != tenLop) continue;

            double tong = 0;
            int dem = 0;
            for (const auto& kq : hs.getDanhSachKetQua())
            {
                if (kq.loaiThi == 1) // Chỉ tính điểm chính thức
                {
                    tong += kq.diem;
                    dem++;
                }
            }
            if (dem > 0)
                bxh.push_back({ hs.getFullName(), hs.getLop(), tong / dem });
        }

        if (bxh.empty())
        {
            SetColor(YELLOW);
            cout << "Chua co sinh vien nao lop " << tenLop << " co diem thi!\n";
            SetColor(WHITE);
            return;
        }

        // Sắp xếp từ cao xuống thấp
        sort(bxh.begin(), bxh.end(), [](const HSScore& a, const HSScore& b)
            { return a.score > b.score; });

        for (int i = 0; i < (int)bxh.size(); i++)
        {
            cout << i + 1 << ". Ho ten: " << bxh[i].fullName << " | Lop: " << bxh[i].className << " | Diem TB: " << fixed << setprecision(2) << bxh[i].score << '\n';
        }
    }

    // ==================== SAVE / LOAD ====================
    void saveTeachers() const
    {
        ofstream fout("giangvien.txt");
        for (const auto& t : teachers)
        {
            string advisorClassName = getAdvisorClassName(t.getId());
            fout << t.getId() << '|' << t.getUsername() << '|' << t.getPassword() << '|' << t.getFullName()
                 << '|' << t.getMon() << '|' << t.getTeacherRoleLabel() << '|' << advisorClassName << '\n';
        }
        fout.close();
    }

    void saveRanking() const
    {
        ofstream fout("ranking.txt");
        for (const auto& r : ranking)
        {
            fout << r.id << '|' << r.fullName << '|' << r.score << '\n';
        }
    }

    void saveStudents() const
    {
        int stt = 1;
        ofstream file("sinhvien_data.txt");
        for (const auto& s : students)
        {
            file << stt++ << "|" << s.getId() << '|' << s.getUsername() << '|' << s.getPassword() << '|' << s.getFullName()
                 << '|' << s.getLop() << '|' << s.getGioiTinh() << '|' << s.getBirthDay() << '|' << s.getSdt()
                 << '|' << s.layDiem() << '|' << s.layMaGV() << '|' << diemMonToString(s.getDiemMon()) << '\n';
        }
        file.close();
    }

    void saveStudentsReadable() const
    {
        ofstream file("sinhvien_report.txt");
        for (const auto& s : students)
        {
            file << "[HS] " << s.getFullName() << "\n";
            file << "| Lop: " << s.getLop() << "\n";
            file << "| Gioi tinh: " << s.getGioiTinh() << "\n";
            file << "| Ngay sinh: " << s.getBirthDay() << "\n";
            file << "| SDT: " << s.getSdt() << "\n\n";
            if (!s.getDiemMon().empty())
            {
                file << "| Diem mon:\n";
                for (const auto& p : s.getDiemMon())
                {
                    file << "    " << p.first << ": " << fixed << setprecision(2) << p.second << "\n";
                }
                file << "\n";
            }
            file << "| Diem TB tat ca mon: " << fixed << setprecision(2) << s.tinhDiemTBTatCaMon() << "\n";
            file << "| Hoc luc: " << s.getHocLuc() << "\n";
            file << "| TeacherID: " << s.layMaGV() << "\n\n";
            file << string(52, '=') << "\n\n";
        }
        file.close();
    }

    void saveData() const
    {
        saveStudents();
        saveStudentsReadable();
        saveTeachers();
        saveClasses();
        saveRooms();
        saveRanking();
        saveKetQua();
        cout << "Luu du lieu thanh cong.\n";
    }

    void loadStudents(const string& fileName)
    {
        students.clear();
        ifstream fin(fileName);
        if (!fin) return;

        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            stringstream ss(line);
            vector<string> tk; string t;
            while (getline(ss, t, '|')) tk.push_back(t);
            if (tk.size() < 11)
            {
                continue;
            }

            SinhVien s;
            int idx = 1;
            s.setId(safeStoi(tk[idx++]));
            s.setUsername(tk[idx++]);
            s.setPassword(tk[idx++]);
            s.setFullName(tk[idx++]);
            s.setlopChuNhiem(tk[idx++]);
            s.setGioiTinh(tk[idx++]);
            s.setBirthDay(tk[idx++]);
            s.setSdt(tk[idx++]);
            s.setDiemTB(safeStod(tk[idx++]));
            s.setTeacherId(safeStoi(tk[idx++]));
            if (idx < (int)tk.size())
            {
                s.setDiemMon(parseDiemMonString(tk[idx++]));
            }
            students.push_back(s);
        }
        fin.close();
        cout << "Tai danh sach sinh vien thanh cong.\n";
    }

    void loadStudentsFromReadableFormat(const string& fileName)
    {
        students.clear();
        ifstream fin(fileName);
        if (!fin) return;

        string line;
        SinhVien s;
        bool inStudent = false;
        vector<pair<string, double>> diemMon;
        int studentId = 1;
        
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();

            string trimmed = trimString(line);
            
            if (trimmed.empty()) continue;

            if (trimmed.find("[HS]") == 0)
            {
                if (inStudent && s.getId() > 0)
                {
                    if (!diemMon.empty())
                    {
                        s.setDiemMon(diemMon);
                    }
                    double tb = 0;
                    if (!diemMon.empty())
                    {
                        for (const auto& p : diemMon) tb += p.second;
                        tb /= diemMon.size();
                        tb = round(tb * 100.0) / 100.0;
                    }
                    s.setDiemTB(tb);
                    students.push_back(s);
                }

                inStudent = true;
                diemMon.clear();
                s = SinhVien();
                s.setId(studentId++);
                s.setUsername("user" + to_string(s.getId()));
                s.setPassword("pass" + to_string(s.getId()));
                
                string hoTen = trimmed.substr(5);
                hoTen = trimString(hoTen);
                s.setFullName(hoTen);
            }
            else if (trimmed.find("| Lop:") == 0)
            {
                string lop = trimmed.substr(6);
                s.setlopChuNhiem(trimString(lop));
            }
            else if (trimmed.find("| Gioi tinh:") == 0)
            {
                string gt = trimmed.substr(12);
                s.setGioiTinh(trimString(gt));
            }
            else if (trimmed.find("| Ngay sinh:") == 0)
            {
                string ns = trimmed.substr(12);
                s.setBirthDay(trimString(ns));
            }
            else if (trimmed.find("| SDT:") == 0)
            {
                string sdt = trimmed.substr(6);
                s.setSdt(trimString(sdt));
            }
            else if (trimmed.find("| TeacherID:") == 0)
            {
                string tid = trimmed.substr(12);
                s.setTeacherId(safeStoi(trimString(tid), 0));
            }
            else if (inStudent && trimmed.find(":") != string::npos && trimmed.find("|") == string::npos)
            {
                size_t colonPos = trimmed.rfind(':');
                string monName = trimmed.substr(0, colonPos);
                string diemStr = trimmed.substr(colonPos + 1);
                monName = trimString(monName);
                diemStr = trimString(diemStr);
                double diem = safeStod(diemStr, 0.0);
                if (!monName.empty()) diemMon.emplace_back(monName, diem);
            }
        }

        if (inStudent && s.getId() > 0)
        {
            if (!diemMon.empty())
            {
                s.setDiemMon(diemMon);
                double tb = 0;
                for (const auto& p : diemMon) tb += p.second;
                tb /= diemMon.size();
                tb = round(tb * 100.0) / 100.0;
                s.setDiemTB(tb);
            }
            students.push_back(s);
        }

        fin.close();
        cout << "Tai danh sach sinh vien (readable format) thanh cong. Tong: " << students.size() << " HS.\n";
    }

    void loadTeachers(const string& fileName)
    {
        teachers.clear();
        ifstream fin(fileName);
        if (!fin) return;

        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            stringstream ss(line);
            vector<string> tk; string t;
            while (getline(ss, t, '|')) tk.push_back(t);
            if (tk.size() < 5) continue;

            GiangVien tObj;
            tObj.setId(safeStoi(tk[0]));
            tObj.setUsername(tk[1]);
            tObj.setPassword(tk[2]);
            tObj.setFullName(tk[3]);
            tObj.setMon(tk[4]);
            if (tk.size() >= 6)
                tObj.setTeacherRole(tk[5]);
            else
                tObj.setTeacherRole("BoMon");
            tObj.normalizeTeacherRole();
            if (tk.size() >= 7)
                tObj.setAdvisorClass(tk[6]);
            else
                tObj.setAdvisorClass("");
            teachers.push_back(tObj);
        }
        fin.close();
        cout << "Tai danh sach giang vien thanh cong.\n";
    }

    void saveClasses() const
    {
        int stt = 1;

        ofstream fout("lophoc.txt");
        for (const auto& c : classes)
        {
            fout << stt++
             << "| ID lop: " << c.id
             << " | Ten lop: " << c.className
             << " |" << c.coVanHocTapId
             << "|" << c.studentCount
             << "|";

            for (int i = 0; i < (int)c.danhSachGiangVien.size(); i++)
            {
                fout << c.danhSachGiangVien[i];
                if (i + 1 < (int)c.danhSachGiangVien.size())
                    fout << ',';
            }
            fout << '\n';
        }
        fout.close();
    }

    void loadClasses(const string& fileName)
    {
        classes.clear();
        ifstream fin(fileName);
        if (!fin) return;
        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            stringstream ss(line);
            vector<string> tk; string t;
            while (getline(ss, t, '|')) tk.push_back(t);
            if (tk.size() < 5) continue;

            ClassInfo c;
            int idx = 1;

            string rawId = tk[idx++];

            size_t posId = rawId.find(":");

            if (posId != string::npos)
            {
                c.id = trimString(rawId.substr(posId + 1));
            }
            else
            {
                c.id = trimString(rawId);
            }
            string rawName = tk[idx++];

            size_t posName = rawName.find(":");

            if (posName != string::npos)
            {
                c.className = trimString(rawName.substr(posName + 1));
            }
            else
            {
                c.className = trimString(rawName);
            }
            c.coVanHocTapId = safeStoi(tk[idx++]);
            c.studentCount = safeStoi(tk[idx++]);

            if (tk.size() >= 6 && !tk[idx].empty())
            {
                stringstream ssTeachers(tk[idx]);
                string teacherToken;
                while (getline(ssTeachers, teacherToken, ','))
                {
                    if (!teacherToken.empty())
                        c.danhSachGiangVien.push_back(safeStoi(teacherToken));
                }
            }

            classes.push_back(c);
        }
            // Sanitize: ensure each teacher is advisor of at most one class
            unordered_set<int> seenAdvisors;
            for (auto &c2 : classes)
            {
                if (c2.coVanHocTapId != 0)
                {
                    if (seenAdvisors.find(c2.coVanHocTapId) == seenAdvisors.end())
                    {
                        seenAdvisors.insert(c2.coVanHocTapId);
                    }
                    else
                    {
                        // Duplicate advisor found; remove assignment from this class
                        cout << "Canh bao: Giang vien ID " << c2.coVanHocTapId << " duoc danh cho nhieu lop. Bo gan cho lop " << c2.className << "\n";
                        c2.coVanHocTapId = 0;
                    }
                }
            }
            recalcTotalStudents();
        fin.close();
    }

    void loadRanking()
    {
        ranking.clear();
        ifstream fin("ranking.txt");
        if (!fin) return;

        string line;
        while (getline(fin, line))
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty()) continue;
            stringstream ss(line);
            vector<string> tk; string t;
            while (getline(ss, t, '|')) tk.push_back(t);
            if (tk.size() < 3) continue;

            RankItem r;
            r.id = safeStoi(tk[0]);
            r.fullName = tk[1];
            r.score = safeStod(tk[2]);
            ranking.push_back(r);
        }
        sortRanking();
    }

    void loadData(QuanLyThi& thi)
    {
        cout << "Chon file sinh vien...\n";
        string studentFile = taiFile();
        if (!studentFile.empty()) loadStudents(studentFile);
        else cout << "Khong chon file sinh vien!\n";

        cout << "Chon file giang vien...\n";
        string teacherFile = taiFile();
        if (!teacherFile.empty()) loadTeachers(teacherFile);
        else cout << "Khong chon file giang vien!\n";

        cout << "Chon file lop hoc...\n";
        string classFile = taiFile();
        if (!classFile.empty()) loadClasses(classFile);
        else cout << "Khong chon file lop hoc!\n";

        cout << "Chon file phong thi...\n";
        string roomFile = taiFile();
        if (!roomFile.empty()) loadRooms(roomFile);
        else cout << "Khong chon file phong thi!\n";

        loadRanking();
        loadKetQua();
        thi.loadDeThi();
        recalcTotalStudents();
        cout << "Tai du lieu thanh cong.\n";
    }

    // ==================== SEARCH STUDENT ====================
    void searchStudent() const
    {
        cout << "\n===== TIM KIEM SINH VIEN =====\n";
        cout << "1. Tim theo ID\n2. Tim theo ho ten\n3. Tim theo lop\n";
        int choice = nhapSo<int>("Chon: ");
        bool found = false;

        if (choice == 1)
        {
            int id = nhapSo<int>("Nhap ID sinh vien: ");
            for (const auto& hs : students)
            {
                if (hs.getId() == id)
                {
                    hs.hienThiThongTin();
                    found = true;
                }
            }
        }
        else if (choice == 2)
        {
            string ten = inputLine("Nhap ho ten (hoac mot phan): ");
            string tenLower = ten;
            transform(tenLower.begin(), tenLower.end(), tenLower.begin(), ::tolower);
            for (const auto& hs : students)
            {
                string nameLower = hs.getFullName();
                transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                if (nameLower.find(tenLower) != string::npos)
                {
                    hs.hienThiThongTin();
                    found = true;
                }
            }
        }
        else if (choice == 3)
        {
            string lop = inputLine("Nhap ten lop: ");
            for (const auto& hs : students)
            {
                if (hs.getLop() == lop)
                {
                    hs.hienThiThongTin();
                    found = true;
                }
            }
        }
        else
        {
            cout << "Lua chon khong hop le.\n";
            return;
        }

        if (!found)
        {
            SetColor(YELLOW);
            cout << "Khong tim thay sinh vien nao.\n";
            SetColor(WHITE);
        }
    }

    // ==================== FIND EXAM ROOMS ====================
    void findExamRooms() const
    {
        cout << "\n===== TIM PHONG THI =====\n";
        if (examRooms.empty())
        {
            cout << "Chua co phong thi nao.\n";
            return;
        }
        cout << "1. Tim theo ID\n2. Tim theo ten phong\n3. Tim phong chua khoa\n";
        int choice = nhapSo<int>("Chon: ");
        bool found = false;

        if (choice == 1)
        {
            int id = nhapSo<int>("Nhap ID phong thi: ");
            for (const auto& r : examRooms)
            {
                if (r.id == id)
                {
                    cout << "ID: " << r.id << " | Ten phong: " << r.tenPhong
                        << " | Suc chua: " << r.sucChua
                        << " | Trang thai: " << (r.locked ? "Khoa" : "Mo") << '\n';
                    found = true;
                }
            }
        }
        else if (choice == 2)
        {
            string ten = inputLine("Nhap ten phong (hoac mot phan): ");
            string tenLower = ten;
            transform(tenLower.begin(), tenLower.end(), tenLower.begin(), ::tolower);
            for (const auto& r : examRooms)
            {
                string nameLower = r.tenPhong;
                transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                if (nameLower.find(tenLower) != string::npos)
                {
                    cout << "ID: " << r.id << " | Ten phong: " << r.tenPhong
                        << " | Suc chua: " << r.sucChua
                        << " | Trang thai: " << (r.locked ? "Khoa" : "Mo") << '\n';
                    found = true;
                }
            }
        }
        else if (choice == 3)
        {
            for (const auto& r : examRooms)
            {
                if (!r.locked)
                {
                    cout << "ID: " << r.id << " | Ten phong: " << r.tenPhong
                        << " | Suc chua: " << r.sucChua << '\n';
                    found = true;
                }
            }
        }
        else
        {
            cout << "Lua chon khong hop le.\n";
            return;
        }

        if (!found)
        {
            SetColor(YELLOW);
            cout << "Khong tim thay phong thi nao.\n";
            SetColor(WHITE);
        }
    }

    // ==================== MENU ====================
    void adminMenu(QuanLyThi& thi)
    {
        int choice;
        do
        {
            SetColor(LIGHT_CYAN);
            cout << "\n╔═════════════════════════════════════════════╗\n";
            SetColor(LIGHT_YELLOW);
            cout << "║              MENU ADMIN TESTPRO             ║\n";
            SetColor(LIGHT_CYAN);
            cout << "╠═════════════════════════════════════════════╣\n";
            SetColor(LIGHT_GREEN);
            cout << "║ [1]  Them giang vien                        ║\n";
            cout << "║ [2]  Xoa giang vien                         ║\n";
            cout << "║ [3]  Sua giang vien                         ║\n";
            cout << "║ [4]  Xem danh sach giang vien               ║\n";
            SetColor(LIGHT_CYAN);
            cout << "╠═════════════════════════════════════════════╣\n";
            SetColor(LIGHT_BLUE);
            cout << "║ [5]  Them lop                               ║\n";
            cout << "║ [6]  Xoa lop                                ║\n";
            cout << "║ [7]  Sua lop                                ║\n";
            cout << "║ [8]  Xem danh sach lop                      ║\n";
            SetColor(LIGHT_CYAN);
            cout << "╠═════════════════════════════════════════════╣\n";
            SetColor(LIGHT_MAGENTA);
            cout << "║ [9]  Thong ke sinh vien / giang vien        ║\n";
            cout << "║ [10] Xuat bao cao thi (CSV)                 ║\n";
            SetColor(LIGHT_CYAN);
            cout << "╠═════════════════════════════════════════════╣\n";
            SetColor(LIGHT_YELLOW);
            cout << "║ [11] Luu du lieu                            ║\n";
            cout << "║ [12] Tai du lieu                            ║\n";
            cout << "║ [13] Tim kiem sinh vien                     ║\n";
            cout << "║ [14] Xem Bang xep hang                      ║\n";
            cout << "║ [15] Quan ly phong thi                      ║\n";
            SetColor(LIGHT_RED);
            cout << "║ [0]  Thoat                                  ║\n";
            SetColor(LIGHT_CYAN);
            cout << "╚═════════════════════════════════════════════╝\n";

            SetColor(WHITE);
            choice = nhapSo<int>("Chon chuc nang: ");

            switch (choice)
            {
            case 1:
                addTeacher();
                break;
            case 2:
                deleteTeacher();
                break;
            case 3:
                editTeacher();
                break;
            case 4:
                viewTeachers();
                break;
            case 5:
                addClass();
                break;
            case 6: deleteClass(); break;
            case 7: editClass(); break;
            case 8: viewClasses(); break;
            case 9: viewSystemStatistics(); break;
            case 10: exportExamReportCSV(thi); break;
            case 11: saveData(); thi.saveDeThi(); break;
            case 12: loadData(thi); break;
            case 13: searchStudent(); break;
            case 14: viewRanking(); break;
            case 15: manageExamRooms(thi); break;
            case 0: cout << "Thoat chuong trinh.\n"; break;
            default: cout << "Lua chon khong hop le.\n";
            }
        } while (choice != 0);
    }
};

void drawLine(int color)
{
    SetColor(color);
    cout << "====================================================\n";
    SetColor(WHITE);
}

//==================== MAIN ====================
int main()
{
    SetConsoleOutputCP(CP_UTF8);

    AdminSystem adminSystem;
    QuanLyNguoiDung ql;
    QuanLyThi thi;

    if (filesystem::exists("sinhvien_data.txt"))
        adminSystem.loadStudents("sinhvien_data.txt");
    else if (filesystem::exists("sinhvien.txt"))
        adminSystem.loadStudentsFromReadableFormat("sinhvien.txt");

    adminSystem.loadTeachers("giangvien.txt");
    adminSystem.loadClasses("lophoc.txt");
    adminSystem.loadRooms("phongthi.txt");
    adminSystem.loadRanking();
    adminSystem.loadKetQua();

    thi.loadDeThi();

    int vaiTro;

    SetColor(LIGHT_CYAN);

    cout << R"(

  /\_/\     ████████╗███████╗███████╗████████╗██████╗ ██████╗  ██████╗ 
 ( >.< )    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝██╔══██╗██╔══██╗██╔═══██╗
  > ^ <        ██║   █████╗  ███████╗   ██║   ██████╔╝██████╔╝██║   ██║
 /     \       ██║   ██╔══╝  ╚════██║   ██║   ██╔═══╝ ██╔══██╗██║   ██║
               ██║   ███████╗███████║   ██║   ██║     ██║  ██║╚██████╔╝ 
               ╚═╝   ╚══════╝╚══════╝   ╚═╝   ╚═╝     ╚═╝  ╚═╝ ╚═════╝ 

)" << endl;

    do
    {
        drawLine(11);
        SetColor(LIGHT_GREEN);
        cout << "        HE THONG QUAN LY THI TRAC NGHIEM\n";
        drawLine(11);
        SetColor(LIGHT_BLUE);
        cout << "        [1] DANG NHAP ADMIN\n";
        SetColor(LIGHT_YELLOW);
        cout << "        [2] DANG NHAP GIANG VIEN\n";
        SetColor(LIGHT_MAGENTA);
        cout << "        [3] DANG NHAP SINH VIEN\n";
        SetColor(LIGHT_RED);
        cout << "        [0] THOAT CHUONG TRINH\n";
        SetColor(WHITE);

        vaiTro = nhapSo<int>("\nNhap lua chon: ");

        if (vaiTro == 1)
        {
            if (adminSystem.loginAdmin())
            {
                adminSystem.adminMenu(thi);
                adminSystem.saveData();
                thi.saveDeThi();
            }
        }
        else if (vaiTro == 2)
        {
            ql.layDanhSach().clear();
            for (auto& t : adminSystem.getTeachers())
            {
                ql.them(&t);
            }

            Person* user = ql.dangNhap();
            if (user != nullptr)
            {
                SetColor(GREEN);
                cout << "Dang nhap thanh cong!\n";
                SetColor(WHITE);

                GiangVien* gv = dynamic_cast<GiangVien*>(user);
                if (gv != nullptr)
                {
                    gv->menu(adminSystem.getStudents(), adminSystem.getClasses(), adminSystem.getRooms(), thi);
                    adminSystem.saveData();
                    thi.saveDeThi();
                }
            }
            else
            {
                SetColor(RED);
                cout << "Dang nhap that bai!\n";
                SetColor(WHITE);
            }
        }
        else if (vaiTro == 3)
        {
            ql.layDanhSach().clear();
            for (auto& hs : adminSystem.getStudents())
            {
                ql.them(&hs);
            }

            Person* user = ql.dangNhap();
            if (user != nullptr)
            {
                SetColor(GREEN);
                cout << "Dang nhap thanh cong!\n";
                SetColor(WHITE);

                SinhVien* hs = dynamic_cast<SinhVien*>(user);
                if (hs != nullptr)
                {
                    hs->menu(thi, adminSystem.getStudents());
                    adminSystem.saveData();
                    thi.saveDeThi();
                }
            }
            else
            {
                SetColor(RED);
                cout << "Dang nhap that bai!\n";
                SetColor(WHITE);
            }
        }
    } while (vaiTro != 0);

    return 0;
}