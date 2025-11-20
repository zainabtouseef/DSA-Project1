import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

void main() {
  runApp(FileSystemApp());
}

class AnatomyColors {
  // DeepSeek Color Scheme
  static const Color deepPurple = Color(0xFF1B103E);
  static const Color darkNavy = Color(0xFF0A0E2A);
  static const Color lightPurple = Color(0xFF9E5BFF);
  static const Color primaryPurple = Color(0xFF6B00FF);
  static const Color buttonPurple = Color(0xFF7A3FFF);
  static const Color accentPurple = Color(0xFFB895FF);

  // Additional colors for anatomy theme
  static const Color neuralBlue = Color(0xFF00D4FF);
  static const Color cardiacRed = Color(0xFFFF4757);
  static const Color musclePink = Color(0xFFFF6B8B);
  static const Color successGreen = Color(0xFF00FF88);
}

class AnatomyGradients {
  static const LinearGradient background = LinearGradient(
    begin: Alignment.topCenter,
    end: Alignment.bottomCenter,
    colors: [AnatomyColors.deepPurple, AnatomyColors.darkNavy],
  );

  static const LinearGradient logo = LinearGradient(
    colors: [AnatomyColors.primaryPurple, AnatomyColors.lightPurple],
    begin: Alignment.topLeft,
    end: Alignment.bottomRight,
  );

  static const RadialGradient glow = RadialGradient(
    radius: 0.7,
    colors: [
      Color(0xAA9E5BFF),
      Color(0x00000000),
    ],
  );
}

class GlassCard extends StatelessWidget {
  final Widget child;
  final VoidCallback? onTap;
  final double borderRadius;

  const GlassCard({
    Key? key,
    required this.child,
    this.onTap,
    this.borderRadius = 20,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        decoration: BoxDecoration(
          borderRadius: BorderRadius.circular(borderRadius),
          color: Colors.white.withOpacity(0.08),
          border: Border.all(
            color: Colors.white.withOpacity(0.2),
            width: 1,
          ),
          boxShadow: [
            BoxShadow(
              color: Colors.black.withOpacity(0.3),
              blurRadius: 20,
              offset: const Offset(0, 10),
            ),
          ],
        ),
        child: ClipRRect(
          borderRadius: BorderRadius.circular(borderRadius),
          child: child,
        ),
      ),
    );
  }
}

class FileSystemApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ODFS File System',
      debugShowCheckedModeBanner: false, // <--- add this line
      theme: ThemeData.dark().copyWith(
        scaffoldBackgroundColor: Colors.transparent,
        primaryColor: AnatomyColors.primaryPurple,
        colorScheme: ColorScheme.dark(
          primary: AnatomyColors.primaryPurple,
          secondary: AnatomyColors.neuralBlue,
          surface: AnatomyColors.deepPurple,
        ),
        inputDecorationTheme: InputDecorationTheme(
          filled: true,
          fillColor: AnatomyColors.deepPurple.withOpacity(0.6),
          border: OutlineInputBorder(
            borderRadius: BorderRadius.circular(16),
            borderSide: BorderSide.none,
          ),
          focusedBorder: OutlineInputBorder(
            borderRadius: BorderRadius.circular(16),
            borderSide: BorderSide(color: AnatomyColors.primaryPurple, width: 2),
          ),
          hintStyle: TextStyle(color: Colors.white54),
          labelStyle: TextStyle(color: AnatomyColors.primaryPurple),
          contentPadding: EdgeInsets.symmetric(horizontal: 20, vertical: 16),
        ),
        textTheme: TextTheme(
          headlineLarge: TextStyle(
            fontSize: 32,
            fontWeight: FontWeight.bold,
            color: Colors.white,
            letterSpacing: 1.2,
          ),
          headlineSmall: TextStyle(
            fontSize: 16,
            color: Colors.white70,
            letterSpacing: 1.1,
          ),
          bodyLarge: TextStyle(
            fontSize: 16,
            color: Colors.white,
          ),
        ),
      ),
      home: LoginScreen(),
    );
  }
}

class ApiService {
  static const String baseUrl = 'http://localhost:8080';

  static Future<Map<String, dynamic>> sendRequest(Map<String, dynamic> request) async {
    try {
      final response = await http.post(
        Uri.parse('$baseUrl/api'),
        headers: {'Content-Type': 'application/json'},
        body: json.encode(request),
      );

      if (response.statusCode == 200) {
        return json.decode(response.body);
      } else {
        return {
          'status': 'error',
          'error_message': 'HTTP Error ${response.statusCode}'
        };
      }
    } catch (e) {
      return {
        'status': 'error',
        'error_message': 'Connection failed: $e'
      };
    }
  }
}

class LoginScreen extends StatefulWidget {
  @override
  _LoginScreenState createState() => _LoginScreenState();
}

class _LoginScreenState extends State<LoginScreen> {
  final TextEditingController _usernameController = TextEditingController();
  final TextEditingController _passwordController = TextEditingController();
  bool _isLoading = false;
  String? _errorMessage;

  Widget _buildBackground() {
    return Stack(
      children: [
        Container(
          decoration: const BoxDecoration(
            gradient: LinearGradient(
              begin: Alignment.topCenter,
              end: Alignment.bottomCenter,
              colors: [
                Color(0xFF1B103E),
                Color(0xFF0D0A1F),
                Color(0xFF050613),
              ],
              stops: [0.0, 0.6, 1.0],
            ),
          ),
        ),
        Positioned(
          top: -100,
          right: -60,
          child: Container(
            width: 350,
            height: 350,
            decoration: const BoxDecoration(
              shape: BoxShape.circle,
              gradient: RadialGradient(
                radius: 0.7,
                colors: [
                  Color(0xAA9E5BFF),
                  Color(0x00000000),
                ],
              ),
            ),
          ),
        ),
        Container(
          decoration: BoxDecoration(
            gradient: LinearGradient(
              begin: Alignment.topCenter,
              end: Alignment.bottomCenter,
              colors: [
                Colors.transparent,
                Colors.black.withOpacity(0.3),
              ],
            ),
          ),
        ),
      ],
    );
  }

  Future<void> _login() async {
    setState(() {
      _isLoading = true;
      _errorMessage = null;
    });

    final request = {
      'operation': 'user_login',
      'request_id': DateTime.now().millisecondsSinceEpoch.toString(),
      'payload': {
        'username': _usernameController.text,
        'password': _passwordController.text,
      },
    };

    final response = await ApiService.sendRequest(request);

    setState(() {
      _isLoading = false;
    });

    if (response['status'] == 'success') {
      final sessionId = response['data']['session_id'];
      Navigator.pushReplacement(
        context,
        MaterialPageRoute(
          builder: (context) => MainAppScreen(sessionId: sessionId),
        ),
      );
    } else {
      setState(() {
        _errorMessage = response['error_message'] ?? 'Login failed';
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Stack(
        children: [
          _buildBackground(),
          Center(
            child: SingleChildScrollView(
              padding: EdgeInsets.all(24),
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  GlassCard(
                    child: Padding(
                      padding: EdgeInsets.all(32),
                      child: Column(
                        children: [
                          // Logo/Header
                          Container(
                            width: 80,
                            height: 80,
                            decoration: BoxDecoration(
                              borderRadius: BorderRadius.circular(20),
                              gradient: const LinearGradient(
                                colors: [AnatomyColors.primaryPurple, AnatomyColors.lightPurple],
                              ),
                            ),
                            child: Icon(
                              Icons.storage,
                              color: Colors.white,
                              size: 40,
                            ),
                          ),
                          SizedBox(height: 20),
                          Text(
                            'ODFS File System',
                            style: TextStyle(
                              fontSize: 28,
                              fontWeight: FontWeight.bold,
                              color: Colors.white,
                            ),
                          ),
                          SizedBox(height: 8),
                          Text(
                            'Secure File Management System',
                            style: TextStyle(
                              color: Colors.white70,
                              fontSize: 16,
                            ),
                          ),
                          SizedBox(height: 32),

                          // Login Form
                          TextField(
                            controller: _usernameController,
                            decoration: InputDecoration(
                              labelText: 'Username',
                              prefixIcon: Icon(Icons.person, color: AnatomyColors.primaryPurple),
                            ),
                          ),
                          SizedBox(height: 16),
                          TextField(
                            controller: _passwordController,
                            decoration: InputDecoration(
                              labelText: 'Password',
                              prefixIcon: Icon(Icons.lock, color: AnatomyColors.primaryPurple),
                            ),
                            obscureText: true,
                          ),
                          SizedBox(height: 24),

                          if (_errorMessage != null)
                            Container(
                              padding: EdgeInsets.all(12),
                              decoration: BoxDecoration(
                                color: AnatomyColors.cardiacRed.withOpacity(0.2),
                                borderRadius: BorderRadius.circular(12),
                                border: Border.all(color: AnatomyColors.cardiacRed),
                              ),
                              child: Row(
                                children: [
                                  Icon(Icons.error, color: AnatomyColors.cardiacRed, size: 20),
                                  SizedBox(width: 8),
                                  Expanded(
                                    child: Text(
                                      _errorMessage!,
                                      style: TextStyle(color: Colors.white),
                                    ),
                                  ),
                                ],
                              ),
                            ),

                          if (_errorMessage != null) SizedBox(height: 16),

                          Container(
                            width: double.infinity,
                            height: 54,
                            decoration: BoxDecoration(
                              borderRadius: BorderRadius.circular(16),
                              gradient: const LinearGradient(
                                colors: [AnatomyColors.primaryPurple, AnatomyColors.lightPurple],
                              ),
                            ),
                            child: _isLoading
                                ? Center(child: CircularProgressIndicator(color: Colors.white))
                                : TextButton(
                              onPressed: _login,
                              child: Text(
                                'Login',
                                style: TextStyle(
                                  color: Colors.white,
                                  fontSize: 16,
                                  fontWeight: FontWeight.bold,
                                ),
                              ),
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class MainAppScreen extends StatefulWidget {
  final String sessionId;

  const MainAppScreen({Key? key, required this.sessionId}) : super(key: key);

  @override
  _MainAppScreenState createState() => _MainAppScreenState();
}

class _MainAppScreenState extends State<MainAppScreen> {
  int _currentIndex = 0;
  String _currentPath = '/';

  Widget _buildBackground() {
    return Stack(
      children: [
        Container(
          decoration: const BoxDecoration(
            gradient: LinearGradient(
              begin: Alignment.topCenter,
              end: Alignment.bottomCenter,
              colors: [
                Color(0xFF1B103E),
                Color(0xFF0D0A1F),
                Color(0xFF050613),
              ],
              stops: [0.0, 0.6, 1.0],
            ),
          ),
        ),
        Positioned(
          top: -100,
          right: -60,
          child: Container(
            width: 350,
            height: 350,
            decoration: const BoxDecoration(
              shape: BoxShape.circle,
              gradient: RadialGradient(
                radius: 0.7,
                colors: [
                  Color(0xAA9E5BFF),
                  Color(0x00000000),
                ],
              ),
            ),
          ),
        ),
        Container(
          decoration: BoxDecoration(
            gradient: LinearGradient(
              begin: Alignment.topCenter,
              end: Alignment.bottomCenter,
              colors: [
                Colors.transparent,
                Colors.black.withOpacity(0.3),
              ],
            ),
          ),
        ),
      ],
    );
  }

  Widget _buildHeader() {
    return Padding(
      padding: const EdgeInsets.only(bottom: 20),
      child: Row(
        children: [
          Container(
            width: 50,
            height: 50,
            decoration: BoxDecoration(
              borderRadius: BorderRadius.circular(15),
              gradient: const LinearGradient(
                colors: [AnatomyColors.primaryPurple, AnatomyColors.lightPurple],
              ),
            ),
            child: Icon(Icons.storage, color: Colors.white),
          ),
          SizedBox(width: 12),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'ODFS File System',
                  style: TextStyle(
                    color: Colors.white,
                    fontSize: 18,
                    fontWeight: FontWeight.bold,
                  ),
                ),
                Text(
                  'Welcome back',
                  style: TextStyle(
                    color: Colors.white70,
                    fontSize: 14,
                  ),
                ),
              ],
            ),
          ),
          IconButton(
            icon: Icon(Icons.logout, color: Colors.white70),
            onPressed: _logout,
          ),
        ],
      ),
    );
  }

  Widget _buildFileBrowser() {
    return Column(
      children: [
        _buildHeader(),
        GlassCard(
          child: Padding(
            padding: EdgeInsets.all(20),
            child: Column(
              children: [
                Row(
                  children: [
                    Expanded(
                      child: Text(
                        'Current Path: $_currentPath',
                        style: TextStyle(
                          color: Colors.white,
                          fontSize: 16,
                        ),
                      ),
                    ),
                    _buildIconButton(
                      icon: Icons.create_new_folder,
                      color: AnatomyColors.neuralBlue,
                      onPressed: () => _showCreateDialog(isDirectory: true),
                    ),
                    SizedBox(width: 8),
                    _buildIconButton(
                      icon: Icons.note_add,
                      color: AnatomyColors.successGreen,
                      onPressed: () => _showCreateDialog(isDirectory: false),
                    ),
                  ],
                ),
                SizedBox(height: 20),
                Container(
                  height: 200,
                  child: ListView(
                    children: [
                      _buildFileItem(
                        name: 'documents',
                        isDirectory: true,
                        onTap: () {},
                      ),
                      _buildFileItem(
                        name: 'pictures',
                        isDirectory: true,
                        onTap: () {},
                      ),
                      _buildFileItem(
                        name: 'readme.txt',
                        isDirectory: false,
                        size: '2.1 KB',
                        onTap: () {},
                      ),
                    ],
                  ),
                ),
              ],
            ),
          ),
        ),
      ],
    );
  }

  Widget _buildIconButton({required IconData icon, required Color color, required VoidCallback onPressed}) {
    return Container(
      width: 44,
      height: 44,
      decoration: BoxDecoration(
        borderRadius: BorderRadius.circular(12),
        color: color.withOpacity(0.2),
        border: Border.all(color: color.withOpacity(0.5)),
      ),
      child: IconButton(
        icon: Icon(icon, color: color, size: 20),
        onPressed: onPressed,
        padding: EdgeInsets.zero,
      ),
    );
  }

  Widget _buildFileItem({required String name, required bool isDirectory, String size = '', required VoidCallback onTap}) {
    return ListTile(
      leading: Container(
        width: 40,
        height: 40,
        decoration: BoxDecoration(
          borderRadius: BorderRadius.circular(10),
          gradient: LinearGradient(
            colors: isDirectory
                ? [AnatomyColors.neuralBlue, AnatomyColors.primaryPurple]
                : [AnatomyColors.musclePink, AnatomyColors.cardiacRed],
          ),
        ),
        child: Icon(
          isDirectory ? Icons.folder : Icons.insert_drive_file,
          color: Colors.white,
          size: 20,
        ),
      ),
      title: Text(
        name,
        style: TextStyle(color: Colors.white),
      ),
      subtitle: size.isNotEmpty ? Text(size, style: TextStyle(color: Colors.white70)) : null,
      trailing: Icon(Icons.arrow_forward_ios, color: Colors.white70, size: 16),
      onTap: onTap,
    );
  }

  Widget _buildUserManagement() {
    return Column(
      children: [
        _buildHeader(),
        GlassCard(
          child: Padding(
            padding: EdgeInsets.all(20),
            child: Column(
              children: [
                Text(
                  'User Management',
                  style: TextStyle(
                    color: Colors.white,
                    fontSize: 20,
                    fontWeight: FontWeight.bold,
                  ),
                ),
                SizedBox(height: 20),
                Container(
                  height: 200,
                  child: ListView(
                    children: [
                      _buildUserItem(
                        name: 'admin',
                        role: 'Administrator',
                        status: 'Active',
                        color: AnatomyColors.successGreen,
                      ),
                      _buildUserItem(
                        name: 'user1',
                        role: 'User',
                        status: 'Active',
                        color: AnatomyColors.successGreen,
                      ),
                      _buildUserItem(
                        name: 'guest',
                        role: 'Guest',
                        status: 'Inactive',
                        color: AnatomyColors.cardiacRed,
                      ),
                    ],
                  ),
                ),
                SizedBox(height: 20),
                Container(
                  width: double.infinity,
                  height: 50,
                  decoration: BoxDecoration(
                    borderRadius: BorderRadius.circular(12),
                    border: Border.all(color: AnatomyColors.neuralBlue),
                  ),
                  child: TextButton(
                    onPressed: _listUsers,
                    child: Text(
                      'Refresh Users',
                      style: TextStyle(
                        color: AnatomyColors.neuralBlue,
                        fontSize: 16,
                        fontWeight: FontWeight.bold,
                      ),
                    ),
                  ),
                ),
              ],
            ),
          ),
        ),
      ],
    );
  }

  Widget _buildUserItem({required String name, required String role, required String status, required Color color}) {
    return ListTile(
      leading: Container(
        width: 40,
        height: 40,
        decoration: BoxDecoration(
          shape: BoxShape.circle,
          gradient: LinearGradient(
            colors: [AnatomyColors.primaryPurple, AnatomyColors.lightPurple],
          ),
        ),
        child: Icon(Icons.person, color: Colors.white, size: 20),
      ),
      title: Text(name, style: TextStyle(color: Colors.white)),
      subtitle: Text(role, style: TextStyle(color: Colors.white70)),
      trailing: Container(
        padding: EdgeInsets.symmetric(horizontal: 8, vertical: 4),
        decoration: BoxDecoration(
          color: color.withOpacity(0.2),
          borderRadius: BorderRadius.circular(8),
          border: Border.all(color: color),
        ),
        child: Text(
          status,
          style: TextStyle(color: color, fontSize: 12),
        ),
      ),
    );
  }

  Widget _buildSystemInfo() {
    return Column(
      children: [
        _buildHeader(),
        FutureBuilder<Map<String, dynamic>>(
          future: _getSystemStats(),
          builder: (context, snapshot) {
            if (snapshot.connectionState == ConnectionState.waiting) {
              return Center(child: CircularProgressIndicator());
            }

            final stats = snapshot.data ?? {};
            return GlassCard(
              child: Padding(
                padding: EdgeInsets.all(20),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      'System Statistics',
                      style: TextStyle(
                        color: Colors.white,
                        fontSize: 20,
                        fontWeight: FontWeight.bold,
                      ),
                    ),
                    SizedBox(height: 20),
                    _buildStatCard('Total Size', '${stats['total_size'] ?? '0'} bytes', AnatomyColors.primaryPurple),
                    _buildStatCard('Used Space', '${stats['used_space'] ?? '0'} bytes', AnatomyColors.musclePink),
                    _buildStatCard('Free Space', '${stats['free_space'] ?? '0'} bytes', AnatomyColors.successGreen),
                    _buildStatCard('Total Files', '${stats['total_files'] ?? '0'}', AnatomyColors.neuralBlue),
                    _buildStatCard('Total Directories', '${stats['total_directories'] ?? '0'}', AnatomyColors.lightPurple),
                  ],
                ),
              ),
            );
          },
        ),
      ],
    );
  }

  Widget _buildStatCard(String title, String value, Color color) {
    return Container(
      margin: EdgeInsets.only(bottom: 12),
      padding: EdgeInsets.all(16),
      decoration: BoxDecoration(
        borderRadius: BorderRadius.circular(12),
        color: color.withOpacity(0.1),
        border: Border.all(color: color.withOpacity(0.3)),
      ),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Text(
            title,
            style: TextStyle(
              color: Colors.white,
              fontSize: 16,
            ),
          ),
          Text(
            value,
            style: TextStyle(
              color: color,
              fontSize: 16,
              fontWeight: FontWeight.bold,
            ),
          ),
        ],
      ),
    );
  }

  // Placeholder methods (same as before)
  Future<void> _createDirectory(String path) async {
    print('Creating directory: $path');
  }

  Future<void> _deleteDirectory(String path) async {
    print('Deleting directory: $path');
  }

  Future<void> _createFile(String path, int size) async {
    print('Creating file: $path with size: $size');
  }

  Future<void> _readFile(String path) async {
    print('Reading file: $path');
  }

  Future<void> _deleteFile(String path) async {
    print('Deleting file: $path');
  }

  Future<void> _listUsers() async {
    print('Listing users');
  }

  Future<void> _getStats() async {
    print('Getting file system stats');
  }

  Future<void> _logout() async {
    print('Logging out');
    Navigator.pushReplacement(
      context,
      MaterialPageRoute(builder: (context) => LoginScreen()),
    );
  }

  Future<Map<String, dynamic>> _getSystemStats() async {
    await Future.delayed(Duration(seconds: 1));
    return {
      'total_size': '1,048,576,000',
      'used_space': '524,288,000',
      'free_space': '524,288,000',
      'total_files': '150',
      'total_directories': '25',
    };
  }

  void _showCreateDialog({required bool isDirectory}) {
    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        backgroundColor: Color(0xFF1A1F3D),
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(20)),
        title: Text(
          isDirectory ? 'Create Directory' : 'Create File',
          style: TextStyle(color: Colors.white),
        ),
        content: TextField(
          decoration: InputDecoration(
            hintText: 'Enter ${isDirectory ? 'directory' : 'file'} name',
            hintStyle: TextStyle(color: Colors.white54),
          ),
          style: TextStyle(color: Colors.white),
          onSubmitted: (value) {
            if (value.isNotEmpty) {
              final fullPath = '$_currentPath/$value';
              if (isDirectory) {
                _createDirectory(fullPath);
              } else {
                _createFile(fullPath, 0);
              }
              Navigator.pop(context);
            }
          },
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context),
            child: Text(
              'Cancel',
              style: TextStyle(color: Colors.white70),
            ),
          ),
          Container(
            decoration: BoxDecoration(
              borderRadius: BorderRadius.circular(12),
              gradient: LinearGradient(
                colors: [AnatomyColors.primaryPurple, AnatomyColors.lightPurple],
              ),
            ),
            child: TextButton(
              onPressed: () => Navigator.pop(context),
              child: Text(
                'Create',
                style: TextStyle(color: Colors.white),
              ),
            ),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Stack(
        children: [
          _buildBackground(),
          SafeArea(
            child: Padding(
              padding: EdgeInsets.all(16),
              child: IndexedStack(
                index: _currentIndex,
                children: [
                  _buildFileBrowser(),
                  _buildUserManagement(),
                  _buildSystemInfo(),
                ],
              ),
            ),
          ),
        ],
      ),
      bottomNavigationBar: GlassCard(
        borderRadius: 0,
        child: Container(
          height: 70,
          padding: EdgeInsets.symmetric(horizontal: 20),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.spaceAround,
            children: [
              _buildNavItem(Icons.folder, 'Files', 0),
              _buildNavItem(Icons.people, 'Users', 1),
              _buildNavItem(Icons.info, 'Stats', 2),
            ],
          ),
        ),
      ),
    );
  }

  Widget _buildNavItem(IconData icon, String label, int index) {
    final isSelected = _currentIndex == index;
    return GestureDetector(
      onTap: () => setState(() => _currentIndex = index),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Container(
            width: 40,
            height: 40,
            decoration: BoxDecoration(
              borderRadius: BorderRadius.circular(12),
              gradient: isSelected
                  ? LinearGradient(
                colors: [AnatomyColors.primaryPurple, AnatomyColors.lightPurple],
              )
                  : null,
              color: isSelected ? null : Colors.transparent,
            ),
            child: Icon(
              icon,
              color: isSelected ? Colors.white : Colors.white70,
              size: 20,
            ),
          ),
          SizedBox(height: 4),
          Text(
            label,
            style: TextStyle(
              color: isSelected ? AnatomyColors.primaryPurple : Colors.white70,
              fontSize: 12,
              fontWeight: isSelected ? FontWeight.bold : FontWeight.normal,
            ),
          ),
        ],
      ),
    );
  }
}